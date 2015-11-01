namespace carl{

enum variableSelectionHeurisics {
		GREEDY_I = 0, 
		//GREEDY_I minimizes the amount of arithmetic operations as fast as possible
		GREEDY_Is = 1,
		//GREEDY_Is does the same as GREEDY_I, but adds a simplifyer at the end.
		GREEDY_II = 2,
		//GREEDY_II minimizes the solution space, by evaluating each monome.
		GREEDY_IIs = 3
		//GREEDY_IIs does the same as GREEDY_II, but adds a simplifyer at the end.
};

#ifdef __VS
#define CONSTEXPR const
#else
#define CONSTEXPR constexpr
#endif

struct strategy
{
	/*
	* selectionType determines which variable selection heuristic is used to create a horner scheme.
	*/
	static CONSTEXPR variableSelectionHeurisics selectionType = GREEDY_I;

	/*
	* defines for GREEDY_II/IIs the default value used to evaluate variables (x = [-targetDiameter,+targetDiameter])
	*/
#ifdef __VS
	static double targetDiameter() { return 0.1; }
#else
	static constexpr double targetDiameter = 0.1;
#endif

	/*
	* Toggles a counter, that monitors the reduction of arithmetic operations by Horner schemes.
	*/
	static CONSTEXPR bool use_arithmeticOperationsCounter = false;
};


}