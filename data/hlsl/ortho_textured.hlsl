struct vertex_in_t
{
   float3 position : POSITION;
   float2 uv : UV;
};

struct vertex_to_fragment_t
{
   float4 position : SV_Position; 
   float2 uv : UV;
};

// MAX SIZE: 4096 Elements (float4), 16 KB
// ALIGNMENT:  must be aligned to 16B,
cbuffer time_cb : register(b1) 
{
   float TIME;
   
   float3 PADDING;
};


Texture2D <float4> tTexture : register(t0);
SamplerState sSampler : register(s0);

float RangeMap( float d0, float d1, float r0, float r1, float v ) 
{
   return ((v - d0) / (d1 - d0)) * (r1 - r0) + r0;
}

// NOP VERTEX FUNCTION
vertex_to_fragment_t VertexFunction( vertex_in_t vertex ) 
{
   vertex_to_fragment_t out_data = (vertex_to_fragment_t)0;

   float width = 320;
   float height = 180;

   // float offset = (sin(TIME) + 1.0f) * 50.0f;

   float transform_x = RangeMap( 0.0f, width, -1.0f, 1.0f, vertex.position.x );
   float transform_y = RangeMap( 0.0f, height, -1.0f, 1.0f, vertex.position.y );

   out_data.position = float4( float3( transform_x, transform_y, vertex.position.z ), 1.0f );
   out_data.uv = vertex.uv;
   return out_data;
}

// COLOR (PIXEL/FRAGMENT) FUNCTION
float4 FragmentFunction( vertex_to_fragment_t data ) : SV_Target0
{
   // ?  Sample Color at Texture Coordinate data.uv
   float4 color = tTexture.Sample( sSampler, data.uv );

   return color;
}




