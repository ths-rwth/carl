
#include "gtest/gtest.h"
#include "../Common.h"

#include "carl/core/VariablePool.h"
#include "carl/util/stringparser.h"

#include "carl/thom2/SignDetermination/SignDetermination.h"
#include "carl/thom2/ThomUtil.h"

#include <iterator>


using namespace carl;

void printCharacter(char c, long unsigned times) {
        for(unsigned i = 0; i < times; i++) {
                std::cout << c;
        }
}


// Demo program for sign determination

TEST(Demo, 1) {
    typedef MultivariatePolynomial<Rational> Polynomial;
    StringParser sp;
    sp.setVariables({"x"});//, "y", "z"});
    
    Variable x = sp.variables().at("x");
    //Variable y = sp.variables().at("y");
    //Variable z = sp.variables().at("z");
    
    Polynomial p;
    Polynomial q;
    
    std::cout << "------------------------------" << std::endl;
    std::cout << "SignDetermination( Der(p), q )" << std::endl;
    std::cout << "------------------------------" << std::endl;
    
    std::cout << "Enter p: ";
    std::string p_string;
    std::cin >> p_string;
    p = sp.parseMultivariatePolynomial<Rational>(p_string);
    std::cout << std::endl;
    
    std::list<Polynomial> der_p = der(p, x, 0, p.degree(x));
    std::cout << "Der(p) = " << std::endl;
    for(const auto& d : der_p) std::cout << d << std::endl;
    std::cout << std::endl;
    
    std::cout << "Enter q: ";
    std::string q_string;
    std::cin >> q_string;
    q = sp.parseMultivariatePolynomial<Rational>(q_string);
    std::cout << std::endl;
    
    std::vector<Polynomial> zeroSet = {q};
    SignDetermination<Rational> sd(zeroSet.begin(), zeroSet.end());
    
    std::cin.ignore();
    
    std::cin.clear();
    std::cin.get();
    for(auto it = der_p.rbegin(); it != der_p.rend(); it++) {
            std::list<SignCondition> signConditions = sd.getSignsAndAdd(*it);
            
            
            
            //std::cout << signConditions << std::endl;
            
            long int iteration = std::distance(der_p.rbegin(), it);
            //std::cout << "iteration = " << iteration << std::endl;
            for(long int i = iteration; i >= 0; i--) {
                    std::cout << p.degree(x) - i << "\t";
            }
            std::cout << std::endl;
            for(int i = 0; i <= iteration; i++) {
                    std::cout << "--------";
            }
            std::cout << std::endl;
            int counter = 1;
            for(const auto& sigma : signConditions) {
                    for(const auto& sign : sigma) {
                            switch(sign) {
                                    case Sign::NEGATIVE : std::cout << "-\t"; break;
                                    case Sign::POSITIVE : std::cout << "+\t"; break;
                                    case Sign::ZERO : std::cout << "0\t"; break;
                            }
                    }
                    std::cout << "sigma" << counter;
                    counter++;
                    std::cout << std::endl;
            }
            std::cout << std::endl;
            
            std::cin.get();
    }
    std::cin.get();
    
}


TEST(Demo, 2) {
    typedef MultivariatePolynomial<Rational> Polynomial;
    StringParser sp;
    sp.setVariables({"x", "y"});
    
    //Variable x = sp.variables().at("x");
    //Variable y = sp.variables().at("y");
    //Variable z = sp.variables().at("z");
    
    int p_size;
    std::vector<Polynomial> p;
    
    std::vector<Polynomial> Z;
    
    std::cout << "-------------------------------------" << std::endl;
    std::cout << "SignDetermination( (p_1,...,p_k), q )" << std::endl;
    std::cout << "-------------------------------------" << std::endl;
    
    std::cout << "Enter k (= length of sequence): ";
    std::cin >> p_size;
    std::cout << "Enter polynomials in sequence: ";
    std::cout << std::endl;
    for(int i = 0; i < p_size; i++) {
        std::string p_string;
        std::cin >> p_string;
        Polynomial tmp = sp.parseMultivariatePolynomial<Rational>(p_string);
        p.push_back(tmp);
    }
    std::cout << std::endl;
    //std::cout << "P = " << p << std::endl;
    
    std::cout << "Enter q: ";
        std::string z_string;
        std::cin >> z_string;
        Polynomial tmp = sp.parseMultivariatePolynomial<Rational>(z_string);
        Z.push_back(tmp);
    std::cout << std::endl;
    
    SignDetermination<Rational> sd(Z.begin(), Z.end());
    
    std::cin.ignore();
    std::cin.clear();
    std::cin.get();
    
    for(auto it = p.rbegin(); it != p.rend(); it++) {
            std::list<SignCondition> signConditions = sd.getSignsAndAdd(*it);
            
            //std::cout << signConditions << std::endl;
            
            long int iteration = std::distance(p.rbegin(), it);
            //std::cout << "iteration = " << iteration << std::endl;
            for(long int i = iteration; i >= 0; i--) {
                    std::cout << p[p_size - i - 1];
                    printCharacter(' ', 5);
//                    std::stringstream ss;
//                    ss << p[p_size - i - 1];
//                    std::string p_string;
//                    ss >> p_string;                   
            }
            std::cout << std::endl;
            for(long int i = iteration; i >= 0; i--) {
                    std::stringstream ss;
                    ss << p[p_size - i - 1];
                    std::string p_string;
                    ss >> p_string;
                    printCharacter('-', p_string.size());
                    printCharacter('-', 5);
            }
            std::cout << std::endl;
         
                    
            for(const auto& sigma : signConditions) {
                    for(long int i = iteration; i >= 0; i--) {
                        std::stringstream ss;
                        ss << p[p_size - i - 1];
                        std::string p_string;
                        ss >> p_string;
                        
                        std::vector<Sign> sigma_vector(sigma.begin(), sigma.end());
                        switch(sigma_vector[sigma.size() - i - 1]) {
                                case Sign::NEGATIVE : std::cout << "-"; break;
                                case Sign::POSITIVE : std::cout << "+"; break;
                                case Sign::ZERO : std::cout << "0"; break;
                        }
                        printCharacter(' ', p_string.size() - 1);
                        printCharacter(' ', 5);
                    }
                    std::cout << std::endl;
            }
            std::cout << std::endl;
            std::cin.get();
    }
    
}

TEST(Demo, 3) {
    typedef MultivariatePolynomial<Rational> Polynomial;
    StringParser sp;
    sp.setVariables({"x", "y"});
    
    //Variable x = sp.variables().at("x");
    //Variable y = sp.variables().at("y");
    //Variable z = sp.variables().at("z");
    
    int p_size;
    std::vector<Polynomial> p;
    
    int z_size;
    std::vector<Polynomial> Z;
    
    std::cout << "-------------------------------------" << std::endl;
    std::cout << "SignDetermination( (p_1,...,p_k), Z )" << std::endl;
    std::cout << "-------------------------------------" << std::endl;
    
    std::cout << "Enter k (= length of sequence): ";
    std::cin >> p_size;
    std::cout << "Enter polynomials in sequence: ";
    std::cout << std::endl;
    for(int i = 0; i < p_size; i++) {
        std::string p_string;
        std::cin >> p_string;
        Polynomial tmp = sp.parseMultivariatePolynomial<Rational>(p_string);
        p.push_back(tmp);
    }
    std::cout << std::endl;
    //std::cout << "P = " << p << std::endl;
    
    std::cout << "Enter size of Z: ";
    std::cin >> z_size;
    std::cout << "Enter polynomials in Z: ";
    std::cout << std::endl;
    for(int i = 0; i < z_size; i++) {
        std::string z_string;
        std::cin >> z_string;
        Polynomial tmp = sp.parseMultivariatePolynomial<Rational>(z_string);
        Z.push_back(tmp);
    }
    std::cout << std::endl;
    
    SignDetermination<Rational> sd(Z.begin(), Z.end());
    
    std::cin.ignore();
    std::cin.clear();
    std::cin.get();
    
    for(auto it = p.rbegin(); it != p.rend(); it++) {
            std::list<SignCondition> signConditions = sd.getSignsAndAdd(*it);
            
            //std::cout << signConditions << std::endl;
            
            long int iteration = std::distance(p.rbegin(), it);
            //std::cout << "iteration = " << iteration << std::endl;
            for(long int i = iteration; i >= 0; i--) {
                    std::cout << p[p_size - i - 1];
                    printCharacter(' ', 5);
//                    std::stringstream ss;
//                    ss << p[p_size - i - 1];
//                    std::string p_string;
//                    ss >> p_string;                   
            }
            std::cout << std::endl;
            for(long int i = iteration; i >= 0; i--) {
                    std::stringstream ss;
                    ss << p[p_size - i - 1];
                    std::string p_string;
                    ss >> p_string;
                    printCharacter('-', p_string.size());
                    printCharacter('-', 5);
            }
            std::cout << std::endl;
         
                    
            for(const auto& sigma : signConditions) {
                    for(long int i = iteration; i >= 0; i--) {
                        std::stringstream ss;
                        ss << p[p_size - i - 1];
                        std::string p_string;
                        ss >> p_string;
                        
                        std::vector<Sign> sigma_vector(sigma.begin(), sigma.end());
                        switch(sigma_vector[sigma.size() - i - 1]) {
                                case Sign::NEGATIVE : std::cout << "-"; break;
                                case Sign::POSITIVE : std::cout << "+"; break;
                                case Sign::ZERO : std::cout << "0"; break;
                        }
                        printCharacter(' ', p_string.size() - 1);
                        printCharacter(' ', 5);
                    }
                    std::cout << std::endl;
            }
            std::cout << std::endl;
            std::cin.get();
    }
    
}