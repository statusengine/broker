#ifndef NEBMODULE_CALLBACK_H
#define NEBMODULE_CALLBACK_H

namespace statusengine {
	class Statusengine;

	template<typename T>
	class NebmoduleCallback {
	public:
		NebmoduleCallback(NEBCallbackType cbType, Statusengine *se, int priority = 0) : cbType(cbType), priority(priority), se(se) {
		}

		NEBCallbackType GetCallbackType() {
			return cbType;
		}

		int GetPriority() {
			return priority;
		}

		void RawCallback(int event_type, void *data) {
			Callback(event_type, reinterpret_cast<T*>(data));
		};
		virtual void Callback(int event_type, T *data) = 0;

	protected:
		Statusengine *se;
		int priority;
		NEBCallbackType cbType;
	};

}

#endif // !NEBMODULE_CALLBACK_H
