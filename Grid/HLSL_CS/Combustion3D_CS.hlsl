Texture3D<min16float4> velocity_in : register(t0);
Texture3D<min16float4> quantity_in : register(t1);

Texture2DArray<uint> obstacle : register(t10);

RWTexture3D<min16float4> quantity_out : register(u0);
RWTexture3D<min16float4> velocity_out : register(u1);

cbuffer constants : register(b1) {
	min16float dt;
}

[numthreads(8, 8, 8)]
void main( uint3 DTid : SV_DispatchThreadID )
{
	velocity_out[DTid.xyz] = velocity_in[DTid.xyz];
	quantity_out[DTid.xyz] = quantity_in[DTid.xyz];

	const min16float4 q = quantity_in[DTid.xyz];
	min16float        temperature = q.g;
	min16float        fuel        = q.b;

	static const min16float combustion_coeff = 200.0f;
	static const min16float temperature_rate = 1700.0f;
	static const min16float smoke_rate = 30.0f;
	static const min16float soot_rate = 60.0f;
	static const min16float burn_rate = 300.0f;
	//static const min16float divergence_coeff = 31.19f;
	//static const min16float divergence_coeff = 0.00015f;
	static const min16float divergence_coeff = 0.0f;

	static const min16float temperature_threshold = 1200.0f;
	static const min16float fuel_threshold = 10.0f;

	if (temperature >= temperature_threshold && fuel >= fuel_threshold) {
		const min16float fuel_before = fuel;
		fuel = max(fuel - burn_rate * dt, 0.0f);
		const min16float coeff = abs(fuel - fuel_before) * combustion_coeff;
		temperature = temperature + temperature_rate * coeff * dt;
		quantity_out[DTid.xyz] = min16float4(q.r + smoke_rate * coeff * dt, temperature, fuel, q.a + soot_rate * coeff * dt);
		velocity_out[DTid.xyz] += min16float4(0.0f, 0.0f, 0.0f, (divergence_coeff * coeff) / dt);
	}
}