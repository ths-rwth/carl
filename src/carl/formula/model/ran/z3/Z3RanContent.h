#pragma once

#include "../../../../config.h"

#ifdef RAN_USE_Z3

#include "../../../../converter/Z3Converter.h"

namespace carl {

    class Z3RanContent {
        private:
            algebraic_numbers::anum mContent;

        public:
            Z3RanContent() {
                z3().anumMan().set(mContent, 0);
            }

            Z3RanContent(const algebraic_numbers::anum& content) {
                z3().anumMan().set(mContent, content);
            }

            Z3RanContent(algebraic_numbers::anum&& content) {
                mContent = content;
            }

            Z3RanContent(const Z3RanContent& content) : Z3RanContent(content.mContent) {
            }

            ~Z3RanContent() {
                z3().free(mContent);
            }

            operator algebraic_numbers::anum&() {
                return mContent;
            }
    };

}

#endif