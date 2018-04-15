#ifndef COMMENT_DATA_CALLBACK_H
#define COMMENT_DATA_CALLBACK_H

#include "nebmodule.h"

#include "NebmoduleCallback.h"

namespace statusengine {
    class CommentDataCallback
        : public NebmoduleCallback<nebstruct_comment_data> {
      public:
        CommentDataCallback(Statusengine *se);

        virtual void Callback(int event_type, nebstruct_comment_data *data);
    };
} // namespace statusengine

#endif // !COMMENT_DATA_CALLBACK_H
