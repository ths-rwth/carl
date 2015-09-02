namespace carl{

enum Strategy {
	GREEDY_I = 0, 
	//GREEDY_I minimizes the amount of arithmetic operations as fast as possible
	GREEDY_Is = 1,
	//GREEDY_Is does the same as GREEDY_I, but adds a simplifyer at the end.
	GREEDY_II = 2,
	//GREEDY_II minimizes the solution space, by evaluating each monome.
	GREEDY_IIs = 3
	//GREEDY_IIs does the same as GREEDY_II, but adds a simplifyer at the end.
};

}