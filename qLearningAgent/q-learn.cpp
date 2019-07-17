/**********************************************************************************/
/*Program q-learn.cpp                                                             */
/*Description: Use q-learning algorithm to train an agent to find an              */
/*optimal path from all the rooms of the 4th floor WWU CS building to             */
/*a specific location on the floor.                                               */
/*Author: David Nachmanson                                                        */
/*Last Date Modified: December 13, 2017                                           */
/*References :  Watkins and Dayan, C.J.C.H., (1992), 'Q-learning.Machine Learning'*/
/* http://www.mnemstudio.org/path-finding-q-learning-tutorial.htm                 */
/**********************************************************************************/
#include <iostream>
#include <cstdlib>
#include <sstream>
#include <fstream>
#include <map>
#include <stack>
#include <string>
#include <vector>
#include <cstring>
#include <math.h>
using namespace std;

const int GRIDHEIGHT = 16;
const int GRIDWIDTH = 52;
const int NUMTARGETS = 56;
const int RANDOM_ACTION_LIKELIHOOD = 90; //from (0 to 100, higher number indicates higher likelihood of chooosing random action when training q-table)
const double GAMMA = .5;
const int HOME_STATE = 429;
int targets[NUMTARGETS] = {0,4,6,59,53,113,105,165,209,215,261,217,313,319,329,371,417,425,431,429,469,
		475,477,486,496,497,504,505,510,511,517,519,573,583,592,593,596,601,603,606,
		611,614,623,625,629,579,633,686,713,714,728,729,731,733,734,736};

/***************************************************************************/
/*Description : Maps 4th floor cs rooms to location on 52 x 16 grid        */
/*Input : none                                                             */
/*Output : a map with rooms as the keys to the grid location (int)         */
/***************************************************************************/
map<string, int> InitializeRoomToGridMap(){
  map <string, int> mapPairs;

  mapPairs["480"] = 0;
  mapPairs["487"] = 4;
  mapPairs["489"] = 6;
  mapPairs["491"] = 59;
  mapPairs["485"] = 53;
  mapPairs["493"] = 113;
  mapPairs["483"] = 105;
  mapPairs["495A"] = 165;
  mapPairs["481"] = 209;
  mapPairs["494"] = 215;
  mapPairs["479"] = 261;
  mapPairs["495"] = 217;
  mapPairs["477"] = 313;
  mapPairs["496"] = 319;
  mapPairs["422A"] = 329;
  mapPairs["498A"] = 371;
  mapPairs["475"] = 417;
  mapPairs["497"] = 425;
  mapPairs["422"] = 431;
  mapPairs["441"] = 429;
  mapPairs["473"] = 469;
  mapPairs["498"] = 475;
  mapPairs["499"] = 477;
  mapPairs["420"] = 486;//has two doors
  mapPairs["418"] = 496;
  mapPairs["416"] = 497;
  mapPairs["414"] = 504;
  mapPairs["412"] = 505;
  mapPairs["408"] = 510;
  mapPairs["406"] = 511;
  mapPairs["404"] = 517;
  mapPairs["401"] = 519;
  mapPairs["471"] = 573;
  mapPairs["451"] = 583;
  mapPairs["423"] = 592;
  mapPairs["421"] = 593;
  mapPairs["419"] = 596;
  mapPairs["417"] = 601;
  mapPairs["415"] = 603;
  mapPairs["413"] = 606;
  mapPairs["407"] = 611;
  mapPairs["405"] = 614;
  mapPairs["X403"] = 623;
  mapPairs["469"] = 625;
  mapPairs["462"] = 629;
  mapPairs["452"] = 579;
  mapPairs["453"] = 633;
  mapPairs["455"] = 686;
  mapPairs["411"] = 713;
  mapPairs["409"] = 714;
  mapPairs["467"] = 728;
  mapPairs["465"] = 729;
  mapPairs["463"] = 731;
  mapPairs["461"] = 733;
  mapPairs["459"] = 734;
  mapPairs["457"] = 736;
  return mapPairs;
}

/***************************************************************************/
/*Description:Fills the R-grid or R-Table with values -1, 0, or 100.If the */
/*action from the state is not valid a -1 is filled in, 0 if valid, 100 if */
/*the action leads to the "home" state                                     */
/*Input : rewards table (2-d int array)                                    */
/*Output : nothing                                                         */
/***************************************************************************/
void InitializeRewardsGrid(int rewards [GRIDHEIGHT * GRIDWIDTH][GRIDHEIGHT * GRIDWIDTH] ){

  for(int i = 0; i < GRIDHEIGHT * GRIDWIDTH; i++){
    for(int j = 0; j < GRIDHEIGHT * GRIDWIDTH; j++){
      rewards[i][j] = -1;
    }
  }

  rewards[0][1] = 0;rewards[1][0] = 0;rewards[1][2] = 0;rewards[2][1] = 0;
	rewards[2][54] = 0;rewards[2][3] = 0;rewards[3][2] = 0;rewards[3][55] = 0;
  rewards[4][56] = 0;rewards[6][58] = 0;rewards[53][54] = 0;rewards[54][2] = 0;
  rewards[54][53] = 0;rewards[54][106] = 0;rewards[55][3] = 0;rewards[55][56] = 0;
  rewards[56][55] = 0;rewards[56][4] = 0;rewards[56][57] = 0;rewards[57][56] = 0;
  rewards[57][58] = 0;rewards[58][6] = 0;rewards[58][110] = 0;rewards[59][111] = 0;
  rewards[105][106] = 0;rewards[106][54] = 0;rewards[106][158] = 0;rewards[110][58] = 0;
  rewards[110][111] = 0;rewards[111][110] = 0;rewards[111][59] = 0;rewards[111][112] = 0;
  rewards[112][111] = 0;rewards[112][113] = 0;rewards[112][164] = 0;rewards[113][112] = 0;
  rewards[113][165] = 0;rewards[0][1] = 0;rewards[0][1] = 0;rewards[0][1] = 0;
  rewards[158][106] = 0;rewards[158][210] = 0;rewards[164][112] = 0;rewards[164][216] = 0;
  rewards[165][113] = 0;rewards[165][217] = 0;rewards[209][210] = 0;rewards[210][209] = 0;
  rewards[210][158] = 0;rewards[210][262] = 0;rewards[215][216] = 0;rewards[216][215] = 0;
  rewards[216][164] = 0;rewards[216][268] = 0;rewards[217][165] = 0;rewards[217][269] = 0;
  rewards[261][262] = 0;rewards[262][261] = 0;rewards[262][210] = 0;rewards[262][314] = 0;
  rewards[268][216] = 0;rewards[268][320] = 0;rewards[268][269] = 0;rewards[269][217] = 0;
  rewards[313][314] = 0;rewards[314][313] = 0;rewards[314][262] = 0;rewards[314][360] = 0;
  rewards[319][320] = 0;rewards[320][319] = 0;rewards[320][268] = 0;rewards[320][372] = 0;
  rewards[360][314] = 0;rewards[360][418] = 0;rewards[371][423] = 0;rewards[372][320] = 0;
  rewards[372][424] = 0;rewards[417][418] = 0;rewards[418][417];rewards[418][360] = 0;
  rewards[418][470] = 0;rewards[423][371] = 0;rewards[423][475] = 0;rewards[424][372] = 0;
  rewards[424][425] = 0;rewards[424][476] = 0;rewards[425][424] = 0;rewards[469][470] = 0;
  rewards[470][469] = 0;rewards[470][418] = 0;rewards[470][522] = 0;rewards[475][423] = 0;
  rewards[475][527] = 0;rewards[475][476] = 0;rewards[476][475] = 0;rewards[476][424] = 0;
  rewards[476][477] = 0;rewards[476][528] = 0;rewards[477][476] = 0;rewards[522][470] = 0;
  rewards[522][574] = 0;rewards[527][475] = 0;rewards[527][579] = 0;rewards[573][574] = 0;
  rewards[574][573] = 0;rewards[574][522] = 0;rewards[574][626] = 0;rewards[579][527] = 0;
  rewards[578][528] = 0;rewards[578][632] = 0;rewards[625][626] = 0;rewards[626][625] = 0;
	rewards[583][531] = 0;rewards[626][574] = 0;rewards[626][679] = 0;rewards[629][682] = 0;
	rewards[632][633] = 0;rewards[632][578] = 0;rewards[632][685] = 0;rewards[633][632] = 0;
	rewards[676][677] = 0;rewards[677][676] = 0;rewards[677][678] = 0;rewards[677][728] = 0;
	rewards[678][677] = 0;rewards[678][729] = 0;rewards[678][679] = 0;rewards[679][678] = 0;
	rewards[679][626] = 0;rewards[679][680] = 0;rewards[680][731] = 0;rewards[680][679] = 0;
	rewards[680][681] = 0;rewards[681][680] = 0;rewards[681][682] = 0;rewards[682][681] = 0;
	rewards[682][629] = 0;rewards[682][683] = 0;rewards[682][733] = 0;rewards[683][682] = 0;
	rewards[683][734] = 0;rewards[683][684] = 0;rewards[684][683] = 0;rewards[684][685] = 0;
	rewards[685][684] = 0;rewards[685][632] = 0;rewards[685][736] = 0;rewards[686][685] = 0;
	rewards[728][677] = 0;rewards[729][678] = 0;rewards[731][680] = 0;rewards[733][682] = 0;
	rewards[734][683] = 0;rewards[736][685] = 0;rewards[528][476] = 0;rewards[528][578] = 0;
	rewards[528][529] = 0;rewards[529][528] = 0;rewards[529][530] = 0;rewards[530][529] = 0;
	rewards[530][531] = 0;rewards[531][530] = 0;rewards[531][583] = 0;rewards[531][532] = 0;
	rewards[532][531] = 0;rewards[532][480] = 0;rewards[532][533] = 0;rewards[533][532] = 0;
	rewards[533][481] = 0;rewards[533][534] = 0;rewards[534][482] = 0;rewards[535][534] = 0;
	rewards[535][536] = 0;rewards[538][486] = 0;rewards[538][590] = 0;rewards[541][593] = 0;
	rewards[544][596] = 0;rewards[548][496] = 0;rewards[549][497] = 0;rewards[549][601] = 0;
	rewards[551][603] = 0;rewards[554][606] = 0;rewards[556][504] = 0;rewards[557][505] = 0;
	rewards[559][611] = 0;rewards[562][510] = 0;rewards[562][614] = 0;rewards[563][511] = 0;
	rewards[569][517] = 0;rewards[571][570] = 0;rewards[571][623] = 0;rewards[571][519] = 0;
	rewards[623][571] = 0;rewards[519][571] = 0;rewards[517][569] = 0;rewards[511][563] = 0;
	rewards[510][562] = 0;rewards[505][557] = 0;rewards[504][556] = 0;rewards[497][549] = 0;
	rewards[496][548] = 0;rewards[486][538] = 0;rewards[590][538] = 0;rewards[590][642] = 0;
	rewards[590][591] = 0;rewards[642][590] = 0;rewards[643][591] = 0;rewards[591][590] = 0;
	rewards[591][643] = 0;rewards[591][592] = 0;rewards[592][591] = 0;rewards[593][541] = 0;
	rewards[596][544] = 0;rewards[601][549] = 0;rewards[603][551] = 0;rewards[606][554] = 0;
	rewards[608][56] = 0;rewards[608][660] = 0;rewards[660][608] = 0;rewards[660][661] = 0;
	rewards[660][713] = 0;rewards[661][660] = 0;rewards[661][714] = 0;rewards[714][661] = 0;
	rewards[713][660] = 0;rewards[611][559] = 0;rewards[614][562] = 0;rewards[480][532] = 0;
	rewards[480][428] = 0;rewards[480][481] = 0;rewards[481][480] = 0;rewards[481][429] = 100;
	rewards[481][482] = 0;rewards[481][533] = 0;rewards[482][481] = 0;rewards[482][430] = 0;
	rewards[482][534] = 0;rewards[428][480] = 0;rewards[428][429] = 100;rewards[430][482] = 0;
	rewards[430][431] = 0;rewards[430][429] = 100;rewards[431][430] = 0;rewards[431][432] = 0;
	rewards[432][431] = 0;rewards[432][433] = 0;rewards[433][432] = 0;rewards[433][381] = 0;
	rewards[381][433] = 0;rewards[381][329] = 0;rewards[329][381] = 0;rewards[429][429] = 100;

  for(int i = 536; i < 571; i++){
    rewards[i][i - 1] = 0;
    rewards[i][i + 1] = 0;
  }
}

/***************************************************************************/
/*Description : Fills the Q-Table with 0's.                                */
/*Input : Q table (2d double array)                                        */
/*Output : nothing                                                         */
/***************************************************************************/
void InitializeQValues(double qValues [GRIDHEIGHT * GRIDWIDTH][GRIDHEIGHT * GRIDWIDTH]){
	srand(time(NULL));
  for(int i = 0; i < GRIDHEIGHT * GRIDWIDTH; i++){
    for(int j = 0; j < GRIDHEIGHT * GRIDWIDTH; j++){
      qValues[i][j] =  0;
    }
  }
}

/***************************************************************************/
/*Description : Initializes all states with 0.(no states have been visited)*/
/*Input : explorationTracker int array                                     */
/*Output : nothing                                                         */
/***************************************************************************/
void  InitializeExplorationTracker(int explorationTracker [GRIDHEIGHT * GRIDWIDTH]){
	for(int i = 0; i < GRIDHEIGHT * GRIDWIDTH; i++){
			explorationTracker[i] = 0;
	}
}

/***************************************************************************/
/*Description : Retrieves all the room names                               */
/*Input : map from room(string) to room location(int) on grid              */
/*Output : vector containing all room names                                */
/***************************************************************************/
vector<string> GetRoomNames(map<string,int> roomToGrid){
	vector<string> rooms;
	for(map<string,int> :: iterator it = roomToGrid.begin(); it != roomToGrid.end(); ++it){
		rooms.push_back(it->first);
	}
	return rooms;
}

/***************************************************************************/
/*Description : prints all room names                                      */
/*Input : vector of room names(strings)                                    */
/*Output : none                                                            */
/***************************************************************************/
void PrintRoomNames(vector<string> roomNames){
	int count = 0;
	vector<string>::iterator it = roomNames.begin();
	cout << '[' << *it;
	++it;
	count++;
	while(it != roomNames.end()){
		if(count > 40){
			cout << '\n';
			count = 0;
		}
		if(count != 0)
			cout << ' ';
		cout << *it;
		++it;
		count++;
	}
	cout << "]\n";
}

/***************************************************************************/
/*Description: given a state, the best possible Q-Value from that state is */
/*found                                                                    */
/*Input : state being evaluated, Q-table(2d double array)                  */
/*Output : max q-value from given state (double)                           */
/***************************************************************************/
double FindMaxQValueFromState(int state, double qValues [GRIDHEIGHT * GRIDWIDTH][GRIDHEIGHT * GRIDWIDTH]){
	double maxAction = 0;
    for(int i = 0; i < GRIDHEIGHT * GRIDWIDTH; i++){
    	if(qValues[state][i] > maxAction)
    		maxAction = qValues[state][i];
    }
    return maxAction;

}

/***************************************************************************/
/*Description : given a state the action with the largest q-value is chosen*/
/*Input : state (int), q-values (2d double array)                          */
/*Output : action to take *a state* (int)                                  */
/***************************************************************************/
int FindOptimalActionFromState(int state, double qValues [GRIDHEIGHT * GRIDWIDTH][GRIDHEIGHT * GRIDWIDTH]){
	int nextAction = 0;
    for(int i = 0; i < GRIDHEIGHT * GRIDWIDTH; i++){
    	if(qValues[state][i] > qValues[state][nextAction])
    		nextAction = i;
    }
    return nextAction;

}

/***************************************************************************/
/*Description : gets possible actions from a given state                   */
/*Input : state being evaluated, R-table (2d int array)                    */
/*Output : Vector of possible actions(ints) from a state                   */
/***************************************************************************/
vector<int> GetPossibleActions(int currentState, int rewards [GRIDHEIGHT * GRIDWIDTH][GRIDHEIGHT * GRIDWIDTH]){
	vector<int> possibleActions;
	for(int i = 0; i < GRIDHEIGHT * GRIDWIDTH; i++){
		if(rewards[currentState][i] != -1){
			possibleActions.push_back(i);
		}
	}
	return possibleActions;
}

/***************************************************************************/
/*Description : finds least explored actions from a set of a actions       */
/*Input : vector of possible actions, array that tracks how many times a   */
/*state has been visited.                                                  */
/*Output : state(int) that has been visited least out of the set           */
/***************************************************************************/
int FindLeastExploredAction(vector<int> possibleActions,int explorationTracker[GRIDHEIGHT * GRIDWIDTH]){
	srand(time(NULL));
	int leastExplored = possibleActions[rand() % possibleActions.size()];
	for(int i = 0; i < possibleActions.size(); i++){
		if(explorationTracker[possibleActions[i]] < explorationTracker[leastExplored]){
			leastExplored = possibleActions[i];
		}
	 }
	 return leastExplored;
}

/***************************************************************************/
/*Description : gets next action for agent                                 */
/*Input : current state, r-table, q-table, exploration tracker, random     */
/*action likelihood(between 0 and 100)                                     */
/*Output : an action to take                                               */
/***************************************************************************/
int GetAction(int currentState, int rewards [GRIDHEIGHT * GRIDWIDTH][GRIDHEIGHT * GRIDWIDTH],
	double qValues [GRIDHEIGHT * GRIDWIDTH][GRIDHEIGHT * GRIDWIDTH],int explorationTracker[GRIDHEIGHT * GRIDWIDTH], int randomActionLikelihood){

  if(randomActionLikelihood > 100 || randomActionLikelihood < 0){
		cerr << "randomActionLikelihood must be between or including 0 and 100\n";
		exit(1);
	}

	srand(time(NULL));
	int random = rand() % 100;
	int choosingRandomly = 0;

	if(random > 100 - (randomActionLikelihood)){
		choosingRandomly = 1;
  }
	vector<int> possibleActions = GetPossibleActions(currentState,rewards);

	if(choosingRandomly) {
		return possibleActions[rand() % possibleActions.size()];
	}
	return FindLeastExploredAction(possibleActions,explorationTracker);
}

/***************************************************************************/
/*Description : trains the agent to find the goal state from various states*/
/*Input : r-table,q-table,explorationTracker                               */
/*Output : none.                                                           */
/***************************************************************************/
void PerformLearningTrials(int rewards [GRIDHEIGHT * GRIDWIDTH][GRIDHEIGHT * GRIDWIDTH],
		double qValues [GRIDHEIGHT * GRIDWIDTH][GRIDHEIGHT * GRIDWIDTH],int explorationTracker[GRIDHEIGHT * GRIDWIDTH],
	int randomActionLikelihood){

		int nextState;
		int currentState;
		double maxQdiff = 1;
	  double qBefore;
		double qAfter;

		cout << "Updating q-values, training agent..\n";
		while(maxQdiff > .00005){
			maxQdiff = 0;

			for(int target = 0; target < NUMTARGETS; target++){
				currentState = targets[target];
        cout << "Training q-table using target " << currentState << "...\n";
				cout << "actions:\n";

				while(currentState != HOME_STATE){

					nextState = GetAction(currentState,rewards,qValues,explorationTracker,randomActionLikelihood);
					cout << nextState << "\n";
					explorationTracker[nextState] += 1;
					qBefore = qValues[currentState][nextState];

					qValues[currentState][nextState] = rewards[currentState][nextState] + (GAMMA * FindMaxQValueFromState(nextState,qValues));

					qAfter = qValues[currentState][nextState];

					if(labs(qAfter - qBefore) > maxQdiff){
						maxQdiff = labs(qAfter - qBefore);
					}
					currentState = nextState;
				}
				cerr << "reached HOME_STATE (room 411)\n";
				InitializeExplorationTracker(explorationTracker);
			}
		}
}

/***************************************************************************/
/*Description : finds path from target to home state using learned q-values*/
/*Also writes that path to an output file.                                 */
/*Input : target,q-table,map of floor                                      */
/*Output : none                                                            */
/***************************************************************************/
int FindOptimalPathFromTarget(int target, double qValues [GRIDHEIGHT * GRIDWIDTH][GRIDHEIGHT * GRIDWIDTH]){
	int currentState = target;
	stack<int> path;
	ofstream outputFile;
	outputFile.open("path.txt");
	int room;
	int pathSize;
	while(currentState != HOME_STATE){
		path.push(currentState);
	  currentState = FindOptimalActionFromState(currentState,qValues);
	}
	path.push(HOME_STATE);
	cout << "path to " << target << " : ";
	pathSize = path.size();

	while(pathSize > 1){
		room = path.top();
		path.pop();
		pathSize--;
		cout << room << ", ";
		outputFile << room << " ";
	}
	 room = path.top();
	 path.pop();
	 cout << room << "\n";
	 outputFile << room;
	 outputFile.close();
}

/***************************************************************************/
/*Description : prompts user to enter a target room so path can be found   */
/*Input : vector of roomnames(strings)                                     */
/*Output : room name entered by user(string)                               */
/***************************************************************************/
string GetUserInput(vector<string> roomNames){
	string input = "";
	cout << "Select a target room from the following list: ";
	PrintRoomNames(roomNames);
	int validInput = 0;

	while(validInput == 0){
		cout << "Your input: ";
		getline(cin,input);

		for(int i = 0; i < roomNames.size(); i++){
			if(input.compare(roomNames[i]) == 0){
				validInput = 1;
				break;
			}
		}

	 if(validInput == 0){
		cerr << "Thats not a room, try again(case sensitive)\n";
	 }
 }

	return input;
}


/***************************************************************************/
/*Description : writes room and associated location to an output file      */
/*Input : map of roomnames(strings) to room location(int) and a vector     */
/*of roomnames(strings)                                                    */
/*Output : none                                                            */
/***************************************************************************/
void WriteRoomInfoToOutput(map<string,int> roomToGrid, vector<string> roomNames){
	ofstream outputFile;
	outputFile.open("roomToGridKey.txt");

	for(int i = 0; i < roomNames.size() - 1; i++){
		outputFile << roomNames[i] << " " << roomToGrid[roomNames[i]] << "\n";
	}
	outputFile << roomNames[roomNames.size() - 1] << " " << roomToGrid[roomNames[roomNames.size() - 1]];
	outputFile.close();
}

/***************************************************************************/
/*Description : trains q-table, asks user to input a target room           */
/*Input : none                                                             */
/*Output : 0 indicating succesful execution                                */
/***************************************************************************/
int main(){
  int rewards [GRIDHEIGHT * GRIDWIDTH][GRIDHEIGHT * GRIDWIDTH];
  double qValues [GRIDHEIGHT * GRIDWIDTH][GRIDHEIGHT * GRIDWIDTH];
	int explorationTracker [GRIDHEIGHT * GRIDWIDTH];
	InitializeExplorationTracker(explorationTracker);
  InitializeRewardsGrid(rewards);
  InitializeQValues(qValues);
  map <string, int> roomToGrid = InitializeRoomToGridMap();
  vector<string> roomNames = GetRoomNames(roomToGrid);
	WriteRoomInfoToOutput(roomToGrid,roomNames);
  PerformLearningTrials(rewards,qValues,explorationTracker,RANDOM_ACTION_LIKELIHOOD);
	cout << "Done training\n";
	FindOptimalPathFromTarget(roomToGrid[GetUserInput(roomNames)],qValues);
	return(0);
}
