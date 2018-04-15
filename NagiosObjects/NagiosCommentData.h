#ifndef NAGIOS_COMMENT_DATA_H
#define NAGIOS_COMMENT_DATA_H

#include "NagiosObject.h"
#include "nebmodule.h"

namespace statusengine {
    class NagiosCommentData : public NagiosObject {
      public:
        NagiosCommentData(const nebstruct_comment_data *commentData);
    };
} // namespace statusengine

#endif // !NAGIOS_COMMENT_DATA_H
