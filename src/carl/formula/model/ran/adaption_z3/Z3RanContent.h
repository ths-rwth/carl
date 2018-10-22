#pragma once

#include "../../../../converter/Z3Converter.h"

namespace carl {

    class Z3RanContent { // TODO is unneccessary copying avoided??
        private:
            algebraic_numbers::anum mContent;

        public:
            Z3RanContent() {
            }

            Z3RanContent(algebraic_numbers::anum&& content) : mContent(content) {
            }

            Z3RanContent(const algebraic_numbers::anum& content) : mContent(content) {
            }

            Z3RanContent(const Z3RanContent& content) : mContent(content.mContent) {
            }

            operator algebraic_numbers::anum&() {
                return mContent;
            }
    };

}