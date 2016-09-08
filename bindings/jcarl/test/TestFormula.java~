import java.util.*;
import jcarl.*;

public class TestFormula {
  static {
    try {
        System.loadLibrary("jcarl");
    } catch (UnsatisfiedLinkError e) {
      System.err.println("Native code library failed to load. See the chapter on Dynamic Linking Problems in the SWIG Java documentation for help.\n" + e);
      System.exit(1);
    }
  }

  public static void main(String argv[]) 
  {
    // Test every class
 Variable v1 = new Variable("v1",VariableType.VT_REAL);	
 Variable v2 = new Variable("v2",VariableType.VT_REAL);	


    Polynomial p1 = new Polynomial(v1);
    Polynomial p2 = new Polynomial(v2);
    FactorizationCache cache = new FactorizationCache();
    FactorizedRationalFunction frf = new FactorizedRationalFunction(new JFactorizedPolynomial(p1,cache),new JFactorizedPolynomial(p2,cache));
	
//SimpleConstraint:
    System.out.println("SimpleConstraint: ");	
    JSimpleConstraint sc1 = new JSimpleConstraint(true);
    JSimpleConstraint sc2 = new JSimpleConstraint(false);
    JSimpleConstraint sc3 = new JSimpleConstraint(p1,Relation.LESS);
 
    System.out.println("sc1: "+sc1.toString());
    System.out.println("sc2: "+sc2.toString());
    System.out.println("sc3: "+sc3.toString());

    System.out.println("sc3.lhs: "+sc3.lhs().toString());
    System.out.println("sc3.rel: "+sc3.rel());


//SimpleConstraintFunc:
    System.out.println("*****************************************************");
    System.out.println("                                      ");
    System.out.println("SimpleConstraintFunc: ");	
    SimpleConstraintFunc scf1 = new SimpleConstraintFunc(true);
    SimpleConstraintFunc scf2 = new SimpleConstraintFunc(false);
    SimpleConstraintFunc scf3 = new SimpleConstraintFunc(frf,Relation.NEQ);
 
    System.out.println("scf1: "+scf1.toString());
    System.out.println("scf2: "+scf2.toString());
    System.out.println("scf3: "+scf3.toString());

    System.out.println("scf3.lhs: "+scf3.lhs().toString());
    System.out.println("scf3.rel: "+scf3.rel());


//Constraint:

   Variable x = new Variable("x",VariableType.VT_BOOL);	


    System.out.println("*****************************************************");
    System.out.println("                                      ");
    System.out.println("Constraint: ");	
    JConstraint c1 = new JConstraint(true);
    JConstraint c2 = new JConstraint(v1,Relation.EQ,new Rational(10));
    JConstraint c3 = new JConstraint(p2,Relation.GEQ);

 
    System.out.println("c1: "+c1.toString());
    System.out.println("c2: "+c2.toString());
    System.out.println("c3: "+c3.toString());

    System.out.println("c2.lhs: "+c2.lhs().toString());
    System.out.println("c2.relation: "+c2.relation());

    VarRationalMap map = new VarRationalMap();
    map.set(v1,new Rational(10));
    map.set(v2,new Rational(-1));	
    System.out.println("evaluate c1: "+c1.satisfiedBy(map));
    System.out.println("evaluate c2 for v1=10: "+c2.satisfiedBy(map)); //TODO: wrong result!!!
    System.out.println("evaluate c3 for v2=-1: "+c3.satisfiedBy(map)); //TODO: here also
	
//Formula:
    System.out.println("*****************************************************");
    System.out.println("                                      ");
    System.out.println("Formula: ");	
    JFormula f1 = new JFormula(x);
    JFormula f2 = new JFormula(c2);
    JFormula f3 = new JFormula(FormulaType.NOT,f2);
    FormulaVector subformulas = new FormulaVector();
    subformulas.add(f1);	
    subformulas.add(f2);
    System.out.println("subformulas[0]: "+subformulas.get(0).toString());
    System.out.println("subformulas[1]: "+subformulas.get(1).toString());
    JFormula f4 = new JFormula(FormulaType.AND,subformulas);

 
    System.out.println("f1: "+f1.toString());
    System.out.println("f2: "+f2.toString());
    System.out.println("f3: "+f3.toString());
    System.out.println("f4: "+f4.toString());



    System.out.println("f4.size: "+f4.size());
    System.out.println("f4.getType: "+f4.getType());
    System.out.println("f4.getItem(1): "+f4.getItem(1).toString());

   


//Operators:
    System.out.println("           ");
    System.out.println("**************************************************");
    System.out.println("Test operators: ");
//Constraint:

    System.out.println("*****************************************************");
    System.out.println("                                      ");
    System.out.println("Constraint: ");	
    System.out.println("NOT c2: "+c2.not_().toString());
    System.out.println("c2 AND c1: "+c2.and_(c1).toString());
    System.out.println("c2 AND f4: "+c2.and_(f4).toString());
    System.out.println("c2 OR c1: "+c2.or_(c1).toString());
    System.out.println("c2 OR f4: "+c2.or_(f4).toString());
    System.out.println("c2 XOR c1: "+c2.xor_(c1).toString());
    System.out.println("c2 XOR f4: "+c2.xor_(f4).toString());

//Formula:
    System.out.println("*****************************************************");
    System.out.println("                                      ");
    System.out.println("Formula: ");	
    System.out.println("NOT f2: "+f2.not_().toString());
    System.out.println("f2 AND c1: "+f2.and_(c1).toString());
    System.out.println("f2 AND f4: "+f2.and_(f4).toString());
    System.out.println("f2 OR c1: "+f2.or_(c1).toString());
    System.out.println("f2 OR f4: "+f2.or_(f4).toString());
    System.out.println("f2 XOR c1: "+f2.xor_(c1).toString());
    System.out.println("f2 XOR f4: "+f2.xor_(f4).toString());







   }








}
