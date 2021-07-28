
struct Vertex {
    int index : SV_VERTEXID;
};

struct V2F {
    float4 position : SV_POSITION;
    float3 color;
};

struct Fragment {
    float4 color : SV_TARGET0;
};

static const float3 g_Positions[] = {
    float3(-1.0, 1.0, 0.0),
    float3(0.0, -1.0, 0.0),
    float3(1.0, 1.0, 0.0),
};

static const float3 g_Colors[] = {
    float3(1.0, 0.0, 0.0),
    float3(0.0, 1.0, 0.0),
    float3(0.0, 0.0, 1.0),
};

void vert(in Vertex i, out V2F o) {
    o.position = float4(g_Positions[i.index], 1.0);
    o.color = g_Colors[i.index];
}

void frag(in V2F i, out Fragment o) {
    o.color = float4(i.color, 1.0);
}
