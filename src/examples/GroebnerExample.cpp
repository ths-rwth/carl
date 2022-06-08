#include <vector>
#include <sstream>

#include <carl-arith/groebner/groebner.h>
#include <carl-arith/groebner/katsura.h>
#include <carl-arith/groebner/cyclic.h>
#include <carl-arith/poly/umvpoly/MultivariatePolynomial.h>
#include <carl-common/debug/Timer.h>

using namespace carl;
const static int MAX_KATSURA = 5;
const static int MAX_CYCLIC = 3;

template <typename C, typename O, typename P>
struct GbBenchmark
{
    GbBenchmark(const std::string& n, const std::vector<MultivariatePolynomial<C, O, P>>& pols)
    {
        name = n;
        polynomials = pols;
    }
    
    std::string name;
    std::vector<MultivariatePolynomial<C, O, P>> polynomials;
};

template<typename C, typename O, typename P>
std::ostream& operator<<(std::ostream& os, const GbBenchmark<C, O, P>& b)
{
    return os << b.name << ": " << b.polynomials << ". ";
}

template<typename C, typename O, typename P>
struct ExecuteBenchmarks {
    typedef MultivariatePolynomial<C, O, P>  Polynomial;

    static std::vector<GbBenchmark<C, O, P>> loadBenchmarks()
    {
        std::vector<GbBenchmark<C, O, P>> res;
        bool verbose = true;
        for(unsigned index = 2; index <= MAX_KATSURA; index++)
        {
            std::stringstream name;
            name << " Katsura " << index;
            std::cout << "Load benchmark: "  << name.str() << std::endl;
            res.push_back(GbBenchmark<C, O, P>(name.str(), carl::benchmarks::katsura<C, O, P>(index)));
            if(verbose) { 
                std::cout << res.back() << std::endl;
            }


        }

        for(unsigned index = 2; index <= MAX_CYCLIC; index++)
        {
             std::stringstream name;
            name << " Cyclic " << index;
            std::cout << "Load benchmark: "  << name.str() << std::endl;
            res.push_back(GbBenchmark<C, O, P>(name.str(), carl::benchmarks::cyclic<C, O, P>(index)));
            if(verbose) { 
                std::cout << res.back() << std::endl;
            }
        }
        return res;

    }

    static std::vector<AbstractGBProcedure<Polynomial>*> loadGbProcedures()
    {
        std::vector<AbstractGBProcedure<Polynomial>*> res;
        res.push_back(new GBProcedure<Polynomial, Buchberger, StdAdding>());
        return res;
    }
};

template<typename C, typename O, typename P>
int execute(std::ostream& os = std::cout)
{
    std::vector<GbBenchmark<C, O, P>> benchmarksets = ExecuteBenchmarks<C, O, P>::loadBenchmarks();
    std::vector<AbstractGBProcedure<MultivariatePolynomial<C, O, P>>*> procedures = ExecuteBenchmarks<C, O, P>::loadGbProcedures();
    
    for(const auto& b : benchmarksets)
    {
        os << "Running benchmark: " << b.name << std::endl;
        int pCount = 0;
        for(auto & p : procedures)
        {
            pCount++;
            os << "\tProcedure: " << pCount << "/" << procedures.size() << std::endl;
            os << "\t\t Adding .. \n";
            os.flush();
            carl::Timer timer;
            for(const auto& pol : b.polynomials)
            {
                std::cout << pol << std::endl;
                p->addPolynomial(pol);
            }
            os << timer << std::endl;
            os << "\t\t Reducing .. \n";
            os.flush();
            timer.reset();
            p->reduceInput();
            os << timer << std::endl;
            os << "\t\t Calculating .. \n";
            os.flush();
            timer.reset();
            p->calculate();
            os << timer << std::endl;
            os << "\t\t Done .. \n";
        }
    }
    
    for(auto & p : procedures)
    {
        delete p;
    }
    return 0;
        
}

int main (int, char** ) 
{
    #ifdef USE_CLN_NUMBERS
    execute<cln::cl_RA, GrLexOrdering, StdMultivariatePolynomialPolicies<NoReasons, NoAllocator>>();
    #else
    execute<mpq_class, GrLexOrdering, StdMultivariatePolynomialPolicies<NoReasons, NoAllocator>>();
    #endif
    
}
