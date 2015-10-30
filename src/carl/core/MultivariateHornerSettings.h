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


struct strategy
{
	/*
	* selectionType determines which variable selcetion heuristic is used to create a horner scheme.
	*/
	static constexpr variableSelectionHeurisics selectionType = GREEDY_I;

	/*
	* defines for GREEDY_II/IIs the default value used to evaluate variables (x = [-targetDiameter,+targetDiameter])
	*/
	static constexpr double targetDiameter = 0.1;

	/*
	* Toggles a counter, that monitors the reduction of arithmetic operations by Horner schemes.
	*/
	static constexpr bool use_arithmeticOperationsCounter = false;
};


}