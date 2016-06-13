/* 
 * File:   Trace.h
 * Author: tobias
 *
 * Created on 10. Juni 2016, 13:04
 */

#pragma once

#include "MultiplicationTable.h"

namespace carl {

template<typename Coeff>
using BaseRepr = typename MultiplicationTable<Coeff>::BaseRepr;

template<typename Coeff>
std::vector<std::pair<int, int>> abc(const _Monomial<Coeff>& c, const MonomialBase<Coeff>& base) {
        std::vector<std::pair<int, int>> res;
        for(int i = 0; i < base.size(); i++) {
                for(int j = 0; j < base.size(); j++) {
                        if(base[i] * base[j] == c) {
                                res.push_back(std::make_pair(i, j));
                        }
                }
        }
        return res;
}

template<typename Coeff>
BaseRepr<Coeff> multiply(const BaseRepr<Coeff>& f, const BaseRepr<Coeff>& g, const MultiplicationTable<Coeff>& tab) {
        assert(f.size() == g.size());
        assert(f.size() == tab.getBase().size());
        
        MonomialBase<Coeff> base = tab.getBase();
        BaseRepr<Coeff> res(base.size(), Coeff(0));
        for(const auto& mon : tab) {
                std::cout << "calculating abc for " << mon << std::endl;
                std::vector<std::pair<int, int>> pairs = abc(mon.first, base);
                std::cout << pairs << std::endl;
                for(const auto& pair : pairs) {
                        for(int i = 0; i < base.size(); i++) {
                                res[i] += mon.second[i] * f[pair.first] * g[pair.second];
                        }
                }
        }
        
        std::cout << "res = " << res << std::endl;
        
        return res;
}

template<typename Coeff>
Coeff trace(const BaseRepr<Coeff>& f, const MultiplicationTable<Coeff>& table) {
        assert(f.size() == table.getBase().size());
        Coeff res(0);
        MonomialBase<Coeff> base = table.getBase();
        for(int i = 0; i < base.size(); i++) {
                for(int j = 0; j < base.size(); j++) {
                        _Monomial<Coeff> ab = base[i] * base[j];
                        res += f[j] * table.get(ab)[i];
                }
        }
        return res;
        
}

} // namespace carl;


