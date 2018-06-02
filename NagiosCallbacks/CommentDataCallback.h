#pragma once

#include "Nebmodule.h"

#include "NebmoduleCallback.h"

namespace statusengine {
    class CommentDataCallback : public NebmoduleCallback {
      public:
        explicit CommentDataCallback(Statusengine *se);

        virtual void Callback(int event_type, void *vdata);
    };
} // namespace statusengine
