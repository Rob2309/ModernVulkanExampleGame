
struct Vertex {
    float3 position;
    float3 color;

    int index : SV_VERTEXID;
};

struct V2F {
    float4 position : SV_POSITION;
    float3 color;
};

struct Fragment {
    float4 color : SV_TARGET0;
};

void vert(in Vertex i, out V2F o) {
    o.position = float4(i.position, 1.0);
    o.color = i.color;
}

void frag(in V2F i, out Fragment o) {
    o.color = float4(i.color, 1.0);
}
