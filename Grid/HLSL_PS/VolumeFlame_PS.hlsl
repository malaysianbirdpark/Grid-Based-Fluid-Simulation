struct PS_IN {
    float3 world_pos : POSITION;
    float3 uvw       : TEXCOORD;
    float4 sv_pos    : SV_Position;
};

Texture3D<min16float4> volume_tex : register(t0);

Texture2D<min16float3> front_texcoord : register(t1);
Texture2D<min16float3> front_world    : register(t2);
Texture2D<min16float3> back_texcoord  : register(t3);
Texture2D<min16float3> back_world     : register(t4);

Texture1D<min16float4> jitter    : register(t5);

SamplerState sampler0 : register(s0);
SamplerState sampler1 : register(s1);

cbuffer PointLight : register(b2)
{
    min16float3 pl_pos;
    min16float3 pl_color;
    min16float  pl_reciprocal_range;
}

cbuffer Dimension : register(b3) 
{
    uint width;
    uint height;
    uint depth;
    float reciprocal_width;
    float reciprocal_height;
    float reciprocal_depth;
}

min16float LightVisibility(min16float3 origin, min16float coeff, min16float3 dir, min16float step_size) {
    min16float visibility = 1.0f;
    [unroll]
    for (int i = 0; i != 20; ++i) {
        const min16float2 src = volume_tex.Sample(sampler0, origin).r;
        visibility *= exp(-src.r * coeff * step_size);
        origin     += dir * step_size;
    }
    return visibility;
}

float WaveLength_nano(const min16float t) {
    static const float b = 2898.0f * 1e3;
    return b / t;
}

float BlackBody(const float lambda, const float t) {
    static const float h = 6.62606957 * 1e-34;
    static const float c = 299792458.0f;
    static const float k = 1.3806488 * 1e-23;

    const float val0 = (2.0f * h * c * c) / ((lambda * lambda) * (lambda * lambda) * lambda);
    const float val1 = 1.0f / max(exp((h * c) / (lambda * k * t)) - 1.0f, 1e-20);

    return val0 * val1;
}

float piecewise_gaussian(const float lambda, const float m, const float sigma_1, const float sigma_2) {
    if (lambda < m)
        return exp(-0.5f * ((lambda - m) * (lambda - m)) / (sigma_1 * sigma_1));
    else
        return exp(-0.5f * ((lambda - m) * (lambda - m)) / (sigma_2 * sigma_2));
}

float x_norm(const float lambda) {
    return 1.056f * piecewise_gaussian(lambda, 599.8f, 37.9f, 31.0f) +
           0.362f * piecewise_gaussian(lambda, 442.0f, 16.0f, 26.7f) -
           0.065f * piecewise_gaussian(lambda, 501.0f, 20.4f, 26.2f);
}

float y_norm(const float lambda) {
    return 0.821f * piecewise_gaussian(lambda, 568.8f, 46.9f, 40.5f) +
           0.286f * piecewise_gaussian(lambda, 530.9f, 16.3f, 31.1f);
}

float z_norm(const float lambda) {
    return 1.217f * piecewise_gaussian(lambda, 437.0f, 11.8f, 36.0f) +
           0.681f * piecewise_gaussian(lambda, 459.0f, 26.0f, 13.8f);
}

float3 GetXYZ(const float t) {
    float3 result = float3(0.0f, 0.0f, 0.0f);
    float lambda_nano = 400.0f;
    for (int i = 0; i != 40; ++i) {
        const float lambda_meter = lambda_nano * 1e-9f;
        result.x += BlackBody(lambda_meter, t) * x_norm(lambda_nano);
        result.y += BlackBody(lambda_meter, t) * y_norm(lambda_nano);
        result.z += BlackBody(lambda_meter, t) * z_norm(lambda_nano);
        lambda_nano += 10.0f;
    }
    const float sum = result.x + result.y + result.z;
    if (sum <= 1e-4)
        return float3(0.0f, 0.0f, 0.0f);
    result.x = result.x / sum;
    result.y = result.y / sum;
    result.z = 1.0f - result.x - result.y;
    return result;
}

float3 ToRGB(float3 xyz) {
    //return float3(
		  //  	3.240479f * xyz.x - 1.537150f * xyz.y - 0.498535f * xyz.z,
		  //  	-0.969256f * xyz.x + 1.875991f * xyz.y + 0.041556f * xyz.z,
		  //  	0.055648f * xyz.x - 0.204043f * xyz.y + 1.057311f * xyz.z
		  // );

    return float3(
                1.656492f * xyz.x - 0.354851f * xyz.y - 0.255038f * xyz.z,
                -0.707196f * xyz.x + 1.655397f * xyz.y + 0.036152f * xyz.z,
                0.051713f * xyz.x - 0.121364f * xyz.y + 1.011530f * xyz.z
		   );
}

float GammaCorrection(const float u) {
    if (u <= 0.0031308f)
        return u * (12.92f * u);
    else
        return u * (1.055 * pow(u, 0.41667f) - 0.055f);
}

float3 ToneMapping(const float3 rgb) {
    float3 result = float3(0.0f, 0.0f, 0.0f);
    result.r = GammaCorrection(rgb.r);
    result.g = GammaCorrection(rgb.g);
    result.b = GammaCorrection(rgb.b);
    return result;
}

float3 GetColor(const float t) {
    const float3 result = GetXYZ(t);
    return ToRGB(result);
}

float4 main(PS_IN input) : SV_Target
{
	static const min16float3 dr = min16float3(reciprocal_width, reciprocal_height, reciprocal_depth) * 0.5f;

    const min16float3 pixel_pos = input.sv_pos.xyz + min16float3(0.5f, 0.5f, 0.0f);

    const min16float3 front_uvw = front_texcoord.Load(pixel_pos);
    const min16float3 back_uvw  = back_texcoord.Load(pixel_pos);
    const min16float  uvw_len   = length(back_uvw - front_uvw);
    const min16float3 uvw_dir   = (back_uvw - front_uvw) / uvw_len;

    const min16float step_size = reciprocal_width * clamp(uvw_len, 1.0f, 2.0f);
    const min16float3 step_uvw = uvw_dir * step_size;

    if (uvw_len < step_size)
        return min16float4(0.0f, 0.0f, 0.0f, 0.0f);

    const min16float3 front_pos = front_world.Load(pixel_pos);
    const min16float3 back_pos  = back_world.Load(pixel_pos);
    const min16float  world_len = length(back_pos - front_pos);
    const min16float3 world_dir = (back_pos - front_pos) / world_len;
    
    min16float3 cur_uvw = front_uvw + 1e-4;
    min16float3 cur_world = front_pos + 1e-4;

    static const min16float3 smoke_albedo = min16float3(0.6f, 0.6f, 0.6f);
    static const min16float3 soot_albedo = min16float3(0.1f, 0.1f, 0.1f);

    float4 dest_color = float4(0.0f, 0.0f, 0.0f, 1.0f);
    float3 src = 0.0f;

    const int iterations = uvw_len / step_size;
    const min16float step_world = world_len / iterations;

    static uint jit = 0;

    float acc_density = 0.0f;

    [loop]
    for (int i = 0; i < iterations + 1; ++i) { 
		const min16float3 jitt = jitter[jit];

		const float3 p0 = cur_uvw + float3( dr.x,  dr.y,  dr.z) + float3(jitt.x, jitt.y, jitt.z);
		const float3 p1 = cur_uvw + float3(-dr.x,  dr.y,  dr.z) + float3(jitt.y, jitt.z, jitt.x);
		const float3 p2 = cur_uvw + float3( dr.x, -dr.y,  dr.z) + float3(jitt.z, jitt.x, jitt.y);
		const float3 p3 = cur_uvw + float3(-dr.x, -dr.y,  dr.z) + float3(jitt.x, jitt.y, jitt.z);
		const float3 p4 = cur_uvw + float3( dr.x,  dr.y, -dr.z) + float3(jitt.y, jitt.z, jitt.x);
		const float3 p5 = cur_uvw + float3(-dr.x,  dr.y, -dr.z) + float3(jitt.z, jitt.x, jitt.y);
		const float3 p6 = cur_uvw + float3( dr.x, -dr.y, -dr.z) + float3(jitt.x, jitt.y, jitt.z);
		const float3 p7 = cur_uvw + float3(-dr.x, -dr.y, -dr.z) + float3(jitt.y, jitt.z, jitt.x);

		float3 val[8];
		val[0] = volume_tex.Sample(sampler0, p0).rga;
		val[1] = volume_tex.Sample(sampler0, p1).rga;
		val[2] = volume_tex.Sample(sampler0, p2).rga;
		val[3] = volume_tex.Sample(sampler0, p3).rga;
		val[4] = volume_tex.Sample(sampler0, p4).rga;
		val[5] = volume_tex.Sample(sampler0, p5).rga;
		val[6] = volume_tex.Sample(sampler0, p6).rga;
		val[7] = volume_tex.Sample(sampler0, p7).rga;

		src = (val[0] + val[1] + val[2] + val[3] + val[4] + val[5] + val[6] + val[7]) * 0.125f;

        const float smoke_density = src.r;
        const float temperature = src.g;
        const float soot_density = src.b;
        acc_density += smoke_density;

		static const float smoke_absorption = 0.1f;
		static const float smoke_scattering = 0.1f;
		static const float soot_absorption = 0.1f;
		static const float soot_scattering = 0.1f;

		float3 dir_to_light = pl_pos - cur_world;
		const float dist_to_light = length(dir_to_light);
		const float dist_to_light_norm = 1.0f - saturate(dist_to_light * pl_reciprocal_range);
		const float att = dist_to_light_norm * dist_to_light_norm;
		dir_to_light = dir_to_light / dist_to_light;
		dir_to_light.y = -dir_to_light.y;
		const float light_visibility = LightVisibility(cur_uvw, soot_scattering, dir_to_light, step_size);

        {
            const float prev_visibility = dest_color.a;
            const float extinction_coeff = smoke_absorption + smoke_scattering;
            const float absorption_coeff = exp(-smoke_density * extinction_coeff * step_size);
            const float3 absorption = (prev_visibility - dest_color.a * absorption_coeff) * soot_albedo * pl_color * light_visibility * att;
            dest_color.a *= absorption_coeff;
            dest_color.rgb += absorption;
        }

        //{
        //    const float prev_visibility = dest_color.a;
        //    const float extinction_coeff = smoke_absorption + smoke_scattering;
        //    const float absorption_coeff = exp(-smoke_density * extinction_coeff * step_size);
        //    const float3 absorption = (prev_visibility - dest_color.a * absorption_coeff) * GetColor(temperature);
        //    dest_color.a *= absorption_coeff;
        //    dest_color.rgb += absorption;
        //}

        {
            const float prev_visibility = dest_color.a;
            const float extinction_coeff = smoke_absorption + smoke_scattering;
            const float emission_coeff = exp(smoke_density * extinction_coeff * step_size);
            //const float3 emission = (dest_color.a * emission_coeff - prev_visibility) * GetColor(temperature) * exp(-acc_density * extinction_coeff * step_size);
            const float3 emission = (dest_color.a * emission_coeff - prev_visibility) * GetColor(temperature);
            dest_color.rgb += emission;
        }

        //{
        //    const float prev_visibility = dest_color.a;
        //    const float extinction_coeff = soot_absorption + soot_scattering;
        //    const float absorption_coeff = exp(-soot_density * extinction_coeff * step_size);
        //    const float emission_coeff = 1.0f - absorption_coeff;
        //    const float3 emission = GetColor(temperature);
        //    dest_color.a *= absorption_coeff;
        //    dest_color.rgb += emission;
        //}

        cur_uvw   += step_uvw;
        cur_world += step_world;
		jit = (jit + 1) - ((jit + 1) & 16);
    }

    dest_color.a = 1.0f - dest_color.a;
    return dest_color;
}