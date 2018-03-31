#ifndef NEBMODULE_CALLBACK_H
#define NEBMODULE_CALLBACK_H

namespace statusengine {

	class Statusengine;

	class NebmoduleCallback {
	public:
		NebmoduleCallback(Statusengine *se);
		virtual void Callback(int event_type, void *data) = 0;

	protected:
		Statusengine *se;
	};



}

#endif // !NEBMODULE_CALLBACK_H
