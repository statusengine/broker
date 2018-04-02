#ifndef NAGIOS_COMMENT_DATA_H
#define NAGIOS_COMMENT_DATA_H

#include "nebmodule.h"
#include "NagiosObject.h"


namespace statusengine {
	class NagiosCommentData : public NagiosObject {
	public:
		NagiosCommentData(const nebstruct_comment_data *commentData);
	};
}

#endif // !NAGIOS_COMMENT_DATA_H
