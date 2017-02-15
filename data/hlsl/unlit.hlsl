//-------------------------------------------------------------------------
// VERTEX INPUT
struct vertex_in_t
{
   float3 position : POSITION;
   float4 tint : TINT;
   float2 uv : UV;
};

//-------------------------------------------------------------------------
// VERTEX TO FRAGMENT DATA
struct vertex_to_fragment_t
{
   float4 position : SV_Position; 
   float4 tint : TINT;
   float2 uv : UV;
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
   out_data.tint = vertex.tint;
   out_data.uv = vertex.uv;
   return out_data;
}

//-------------------------------------------------------------------------
// COLOR (PIXEL/FRAGMENT) FUNCTION
float4 FragmentFunction( vertex_to_fragment_t data ) : SV_Target0
{
   // ?  Sample Color at Texture Coordinate data.uv
   float4 color = tTexture.Sample( sSampler, data.uv );

   float4 final_color = data.tint * color;
   return final_color;
}




