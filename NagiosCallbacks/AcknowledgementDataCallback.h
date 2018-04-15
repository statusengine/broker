#ifndef ACKNOWLEDGEMENTDATA_CALLBACK_H
#define ACKNOWLEDGEMENTDATA_CALLBACK_H

#include "nebmodule.h"

#include "NebmoduleCallback.h"

namespace statusengine {
    class AcknowledgementDataCallback
        : public NebmoduleCallback<nebstruct_acknowledgement_data> {
      public:
        AcknowledgementDataCallback(Statusengine *se);

        virtual void Callback(int event_type,
                              nebstruct_acknowledgement_data *data);
    };
} // namespace statusengine

#endif // !ACKNOWLEDGEMENTDATA_CALLBACK_H
