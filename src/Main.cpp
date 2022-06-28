#include <Seidon.h>
#include <ExtensionEntryPoint.h>

struct SampleComponent
{
    int a;
};

class SampleSystem : public Seidon::System
{
public:
    void Init() override
    {
        
    }

    void Update(float deltaTime) override
    {

    }

    void Destroy() override
    {

    }
};

void Init(Seidon::Application& app)
{
    app.RegisterComponent<SampleComponent>()
        .AddMember("Sample Field", &SampleComponent::a);

    app.RegisterSystem<SampleSystem>();
}

void Destroy(Seidon::Application& app)
{
    app.UnregisterComponent<SampleComponent>();

    app.UnregisterSystem<SampleSystem>();
}