#pragma once

#include "../../../../converter/Z3Converter.h"

namespace carl {

    class Z3RanContent { // TODO is unneccessary copying avoided??
        private:
            algebraic_numbers::anum mContent;

        public:
            Z3RanContent() {
                z3().anumMan().set(mContent, 0);
            }

            Z3RanContent(const algebraic_numbers::anum& content) {
                z3().anumMan().set(mContent, content);
            }

            Z3RanContent(const Z3RanContent& content) : Z3RanContent(content.mContent) {
            }

            operator algebraic_numbers::anum&() {
                return mContent;
            }
    };

}