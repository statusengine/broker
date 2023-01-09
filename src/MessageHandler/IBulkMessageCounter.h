#pragma once

namespace statusengine {

    class IBulkMessageCounter {
    public:
        virtual void IncrementCounter() = 0;
    };

}
