
struct Vertex {
    float3 position;
    float3 color;
};

struct V2F {
    float4 position : SV_POSITION;
    float3 color;
};

struct Fragment {
    float4 color : SV_TARGET0;
};

struct Transform {
    float4x4 model2world;
    float4x4 projection;
};
[[vk::push_constant]] ConstantBuffer<Transform> u_Transform;

void vert(in Vertex i, out V2F o) {
    o.position = float4(i.position, 1.0) * u_Transform.model2world * u_Transform.projection;
    o.color = i.color;
}

void frag(in V2F i, out Fragment o) {
    o.color = float4(i.color, 1.0);
}
