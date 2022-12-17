#include "App.h"

ref class Direct3DApplicationSource sealed : Windows::ApplicationModel::Core::IFrameworkViewSource {
public:
	virtual Windows::ApplicationModel::Core::IFrameworkView^ CreateView() { return ref new the_one_uwp::App(); }
};

// The main function is only used to initialize our IFrameworkView class.
[Platform::MTAThread]
int main(Platform::Array<Platform::String^>^) {
	auto direct3DApplicationSource = ref new Direct3DApplicationSource();
	Windows::ApplicationModel::Core::CoreApplication::Run(direct3DApplicationSource);
	return 0;
}

