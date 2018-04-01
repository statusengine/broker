#ifndef NEBMODULE_CALLBACK_H
#define NEBMODULE_CALLBACK_H

namespace statusengine {

	template<typename T>
	class NebmoduleCallback {
	public:
		void RawCallback(int event_type, void *data) {
			Callback(event_type, reinterpret_cast<T*>(data));
		};
		virtual void Callback(int event_type, T *data) = 0;
	};

}

#endif // !NEBMODULE_CALLBACK_H
