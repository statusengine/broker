#include "NagiosCommentData.h"

namespace statusengine {

    NagiosCommentData::NagiosCommentData(const nebstruct_comment_data *commentData) {
        SetData<>("type", commentData->type);
        SetData<>("flags", commentData->flags);
        SetData<>("attr", commentData->attr);
        SetData<>("timestamp", commentData->timestamp.tv_sec);

        json_object *comment = json_object_new_object();

        SetData<>("host_name", commentData->host_name, comment);
        SetData<>("service_description", commentData->service_description, comment);
        SetData<>("author_name", commentData->author_name, comment);
        SetData<>("comment_data", commentData->comment_data, comment);
        SetData<>("comment_type", commentData->comment_type, comment);
        SetData<>("entry_time", commentData->entry_time, comment);
        SetData<>("persistent", commentData->persistent, comment);
        SetData<>("source", commentData->source, comment);
        SetData<>("entry_type", commentData->entry_type, comment);
        SetData<>("expires", commentData->expires, comment);
        SetData<>("expire_time", commentData->expire_time, comment);
        SetData<>("comment_id", commentData->comment_id, comment);

        SetData<>("comment", comment);
    }
} // namespace statusengine
