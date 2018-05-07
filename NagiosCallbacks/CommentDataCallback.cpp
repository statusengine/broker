#include "CommentDataCallback.h"

#include "NagiosObjects/NagiosCommentData.h"
#include "Statusengine.h"

namespace statusengine {
    CommentDataCallback::CommentDataCallback(Statusengine *se) : NebmoduleCallback(NEBCALLBACK_COMMENT_DATA, se) {}

    void CommentDataCallback::Callback(int event_type, nebstruct_comment_data *data) {
        auto statusData = NagiosCommentData(data);
        se->SendMessage("statusngin_comments", statusData.ToString());
    }
} // namespace statusengine
