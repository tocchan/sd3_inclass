//-------------------------------------------------------------------------
// VERTEX INPUT
struct vertex_in_t
{
   float3 position : POSITION;
   float3 normal : NORMAL;
   float4 tint : TINT;
   float2 uv : UV;
};

//-------------------------------------------------------------------------
// VERTEX TO FRAGMENT DATA
struct vertex_to_fragment_t
{
   float4 position : SV_Position; 
   float3 normal : NORMAL;
   float4 tint : TINT;
   float2 uv : UV;

   float3 world_position : WORLD;
};

//-------------------------------------------------------------------------
// CONSTANT BUFFERS
cbuffer matrix_cb : register(b0)
{
   float4x4 MODEL;
   float4x4 VIEW;
   float4x4 PROJECTION;
};

// MAX SIZE: 4096 Elements (float4), 16 KB
// ALIGNMENT:  must be aligned to 16B,
cbuffer time_cb : register(b1) 
{
   float GAME_TIME;
   float SYSTEM_TIME;
   float GAME_FRAME_TIME;
   float SYSTEM_FRAME_TIME;
};

cbuffer light_cb : register(b2)
{
   float4 AMBIENT;         // <r, g, b, intensity>

   // POINT LIGHT, DOT3
   float4 LIGHT_COLOR;     // <r, g, b, intensity>
   float4 LIGHT_POSITION;
   float4 ATTENUATION;
};


Texture2D <float4> tTexture : register(t0);
SamplerState sSampler : register(s0);

//-------------------------------------------------------------------------
// NOP VERTEX FUNCTION
vertex_to_fragment_t VertexFunction( vertex_in_t vertex ) 
{
   vertex_to_fragment_t out_data = (vertex_to_fragment_t)0;

   float4 model_position = float4( vertex.position, 1.0f );
   float4 world_position = mul( model_position, MODEL );
   float4 view_position = mul( world_position, VIEW );
   float4 clip_position = mul( view_position, PROJECTION );

   out_data.position = clip_position;
   out_data.normal = mul( float4(vertex.normal, 0.0f), MODEL ).xyz; // W get sa 0, so as not to take on the translation of the model matrix
   out_data.tint = vertex.tint;
   out_data.uv = vertex.uv;
   out_data.world_position = world_position.xyz;
   return out_data;
}

float4 NormalAsColor( float3 n )
{
   return float4( (n + 1.0f) * .5f, 1.0f );
}

//-------------------------------------------------------------------------
// COLOR (PIXEL/FRAGMENT) FUNCTION
float4 FragmentFunction( vertex_to_fragment_t data ) : SV_Target0
{
   // ?  Sample Color at Texture Coordinate data.uv
   float4 color = tTexture.Sample( sSampler, data.uv );
   float4 diffuse = color * data.tint;

   // renormalize - the interpolation step may have skewed it
   float3 normal = normalize(data.normal);

   // light factor

   // first, calculate ambient (just added in - once per fragment)
   float4 ambient_factor = float4( AMBIENT.xyz * AMBIENT.w, 1.0f );

   // point light factor - light factors are calculated PER LIGHT and added 
   // to the ambient factor to get your final diffuse factor
   float3 vector_to_light = LIGHT_POSITION.xyz - data.world_position;
   float distance_to_light = length(vector_to_light);
   float3 dir_to_light = vector_to_light / distance_to_light;

   float dot3 = saturate( dot( dir_to_light, normal ) );
   float attenuation = LIGHT_COLOR.w / (ATTENUATION.x 
      + distance_to_light * ATTENUATION.y
      + distance_to_light * distance_to_light * ATTENUATION.z);

   float4 light_color = float4( LIGHT_COLOR.xyz, 1.0f ); 
   float4 light_factor = light_color * dot3 * attenuation;

   // calculate final diffuse factor by adding all light factors to the ambiant factor
   float4 diffuse_factor = saturate(ambient_factor + light_factor);

   // calculate final color
   float4 final_color = diffuse_factor * diffuse;
   return final_color;
}




