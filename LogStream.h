#ifndef LOGSTREAM_H
#define LOGSTREAM_H

#include <sstream>

namespace statusengine {
    std::ostream &eom(std::ostream &os);

    std::ostream &eoem(std::ostream &os);

    std::ostream &eowm(std::ostream &os);
} // namespace statusengine

#endif // !LOGSTREAM_H
