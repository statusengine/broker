#pragma once

#include "nebmodule.h"

#include "NebmoduleCallback.h"

namespace statusengine {
    class CommentDataCallback : public NebmoduleCallback<nebstruct_comment_data> {
      public:
        explicit CommentDataCallback(Statusengine *se);

        virtual void Callback(int event_type, nebstruct_comment_data *data);
    };
} // namespace statusengine
