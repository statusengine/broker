#pragma once

#include "NagiosObject.h"
#include "nebmodule.h"

namespace statusengine {
    class NagiosCommentData : public NagiosObject {
      public:
        explicit NagiosCommentData(const nebstruct_comment_data *commentData);
    };
} // namespace statusengine
