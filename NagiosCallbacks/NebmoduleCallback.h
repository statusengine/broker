#ifndef NEBMODULE_CALLBACK_H
#define NEBMODULE_CALLBACK_H

#include <functional>

#include "chacks.h"

namespace statusengine {
    class Statusengine;

    template <typename T> class NebmoduleCallback {
      public:
        explicit NebmoduleCallback(NEBCallbackType cbType, Statusengine *se, int priority = 0)
            : cbType(cbType), priority(priority), se(se) {}

        NEBCallbackType GetCallbackType() { return cbType; }

        int GetPriority() { return priority; }

        void RawCallback(int event_type, void *data) { Callback(event_type, reinterpret_cast<T *>(data)); };
        virtual void Callback(int event_type, T *data) = 0;

        int (*GetCallbackFunction())(int, void *) {
            return fnptr<int(int, void *)>([this](int event_type, void *data) -> int {
                RawCallback(event_type, data);
                return 0;
            });
        }

      protected:
        Statusengine *se;
        int priority;
        NEBCallbackType cbType;
    };

} // namespace statusengine

#endif // !NEBMODULE_CALLBACK_H
