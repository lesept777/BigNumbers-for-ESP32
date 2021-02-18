#include "BigNumbers.h"

bigNumber::bigNumber() {
	
}

bigNumber::~bigNumber() {
	
}

void bigNumber::set (double mant, std::vector<double> const &expoVec, int nbexpo) {
	double expo[MAXFLOORS] = {0};
	uint8_t i = 0;
	for (double x : expoVec) expo[i++] = x;
	this->set(mant, expo, nbexpo);
}

void bigNumber::set(double mant, double* expo, int nbexpo) {
	// Serial.printf("entree set: floors = %d\n", nbexpo);
	// Serial.printf("entree set: mant = %f\n", mant);
	// for (byte i = 0; i < nbexpo; i++) Serial.printf("entree set: expo %d = %f\n", i, expo[i]);

	if (nbexpo == 0) {
		Serial.println ("Set : error, 0 floor!");
		return;
	}

	if (mant == 0.0f) {
		this->mant = 0;
		this->expo[0] = 0;
		this->floors = 0;
		return;
	}

	// Reduce the last floors
	if (nbexpo == 1) {
		if (mant > 0) {
			double x = log10(mant) + expo[0];
			if (x < MAXPREC) {
				mant = pow(10.0f, x);
				expo[0] = 0;
				nbexpo = 0;
			}			
		}
	} else if (nbexpo == 2) {
		double x = log10(expo[0]) + expo[1];
		if (x < MAXPREC) {
			expo[0] = pow(10.0f, x);
			nbexpo = 1;
			x = log10(mant) + expo[0];
			if (x < MAXPREC) {
				mant = pow(10.0f, x);
				expo[0] = 0;
				nbexpo = 0;
			}
		}
	} else {
		double x = log10(expo[nbexpo - 2]) + expo[nbexpo - 1];
		if (x < MAXPREC) {
			expo[nbexpo - 2] = pow(10.0f, x);
			nbexpo -= 1;
		}
	}

	this->floors = (nbexpo == 0) ? 0 : nbexpo - 1;

	if (this->floors > 0) {
		while (expo[this->floors - 1] >= 10.0f) {
			expo[this->floors - 1] /= 10.0f;
			++ expo[this->floors];
		}		
	}

	for (int i = this->floors - 1; i >= 0; i--) {
		while (expo[i] > 10.0f) expo[i] /= 10.0f;
	}

	if (mant > 0) l_mant = (int)log10(mant);
	while (mant >= 10.0f) {
		mant /= 10.0f;
		if (this->floors == 0) ++expo[0];
	}
	while (mant < 1.0f && mant > 0) {
		mant *= 10.0f;
		if (this->floors == 0) --expo[0];
	}

	this->mant = mant * pow(10.0f, MAXPREC);
	if (mant > 0) this->l_mant = log10(mant);
	for (byte i = 0; i < this->floors; i++) this->expo[i] = expo[i] * pow(10.0f, MAXPREC);
	for (byte i = 0; i < this->floors; i++) this->l_exp[i] = log10(expo[i]);
	this->expo[this->floors] = expo[this->floors];
	this->l_exp[this->floors] = log10(expo[this->floors]);

	// clean values ending by 9999
	if (this->mant % 1000000 == 999999) this->mant ++;
	for (byte i = 0; i <= this->floors; i++) if (this->expo[i] % 1000000 == 999999) this->expo[i] ++;

	// Serial.println("fin set");
	// Serial.printf("set: floors = %d\n", this->floors);
	// Serial.printf("set: mant = %lld\n", this->mant);
	// for (byte i = 0; i < this->floors; i++) Serial.printf("set: expo %d = %lld\n", i, this->expo[i]);
}

bool bigNumber::operator==(bigNumber const& N) const {
	if (this->mant   != N.mant) return false;
	if (this->floors != N.floors) return false;
	for (uint8_t i = 0; i < this->floors; i++) 
		if (this->expo[i] != N.expo[i]) return false;
	return true;
}

bool bigNumber::operator!=(bigNumber const& N) const {
	return !(*this == N);
}

bool bigNumber::operator<(bigNumber const& N) const {
	if (this->floors < N.floors) return true;
	if (N.floors < this->floors) return false;
	for (uint8_t i = 0; i < this->floors; i++) {
		if (this->expo[i] < N.expo[i]) return true;
		if (N.expo[i] < this->expo[i]) return false;
	}
	if (this->mant < N.mant) return true;
	return false;
}

bool bigNumber::operator<=(bigNumber const& N) const {
	return (*this < N || *this == N);
}

bool bigNumber::operator>(bigNumber const& N) const {
	return !(*this <= N);
}

bool bigNumber::operator>=(bigNumber const& N) const {
	return !(*this < N);
}

void bigNumber::operator+=(bigNumber & N) {
	// Serial.println("add");
	// Serial.printf("1: %lld exp %lld floors %d\n", this->mant,this->expo[0],this->floors);
	// Serial.printf("2: %lld exp %lld floors %d\n", N.mant,N.expo[0],N.floors);

	if (this->floors < 2 && N.floors < 2) { // perform real addition
		if (this->floors == 1 && N.floors == 0) return;
		if (this->floors == 0 && N.floors == 1) {*this = N.extract(0); return;}
		if (this->floors == 0 && N.floors == 0) {
			if (this->expo[0] > N.expo[0] + MAXPREC) return;
			if (N.expo[0] > this->expo[0] + MAXPREC) {*this = N.extract(0); return;}
			int delta = this->expo[0] - N.expo[0];
			int floors = 1;
			double mant = (double)(this->mant / pow(10, MAXPREC))
						+ (double)(N.mant / pow(10, delta + MAXPREC));
			double expo[MAXFLOORS] = {0};
			expo[0] = this->expo[0];
			this->set(mant, expo, floors);
			return;
		}
	}
	// Case of same exponents
	if (this->extract(1) == N.extract(1)) {
		int floors = this->floors + 1;
		double mant = (double)(this->mant / pow(10, MAXPREC)) 
					+ (double)(N.mant / pow(10, MAXPREC));
		double expo[MAXFLOORS] = {0};
		for (byte i = 0; i < floors; i++) {
			if (i == floors - 1) expo[i] = this->expo[i];
			else expo[i] = this->expo[i] / pow(10, MAXPREC);
		} 
		this->set(mant, expo, floors);
		return;
	}
	// otherwise return the biggest number
	if (*this < N) *this = N.extract(0);
}


void bigNumber::operator*=(bigNumber & N) {
	// Serial.println("mult");
	// Serial.printf("1: %lld exp %lld floors %d\n", this->mant,this->expo[0],this->floors);
	// Serial.printf("2: %lld exp %lld floors %d\n", N.mant,N.expo[0],N.floors);

	int floors = max(this->floors, N.floors);
	double mant = (this->mant / pow(10.0f, MAXPREC)) * N.mant;
	double expo[MAXFLOORS] = {0};
	bigNumber BN1 = this->extract(1);
	bigNumber BN2 = N.extract(1);
	BN1 = BN1 + BN2;

	this->push(BN1,1);
	this->mant = mant;
	this->floors = floors;
}

// Compute A ^ B = 10 ^ (B * log10 (A))
void bigNumber::bigPower (bigNumber & B) {
	double mant = log10(this->mant / pow(10.0f, MAXPREC)); // 'this' is A
	double expo[MAXFLOORS] = {0};
	int floors = 1;
	bigNumber D;
	D.set(mant, expo, floors);
	bigNumber E = this->extract(1); // log10(A)
	E += D;
	E = B * E; 						// B * log10 (A)
	this->push(E, 1);				// 10 ^ (B * log10 (A))
	if (this->floors == 0) {
		float x = E.mant / pow(10, MAXPREC - E.expo[0]);
		x = x - (int)(x);
		this->mant = pow(10, x + MAXPREC);		
	}
}

bigNumber bigNumber::extract(uint8_t begin, uint8_t end) {
	bigNumber B;
	char t[30];
	// Serial.printf("extract floors = %d begin = %d\n",this->floors, begin);  delay(30);
	// Special case of 0 floors
	if (this->floors == 0) {
		double mant;
		double expo[MAXFLOORS] = {0};
		if (begin == 0) {
			mant = this->mant / pow(10.0f,MAXPREC);
			expo[0] = this->expo[0];
		} else {
			mant = this->expo[0];
			expo[0] = 0.0f;
		}
		int floors = 1;
		B.set(mant, expo, floors);
		return B;
	}

	bool theEnd = false;
	if (end > this->floors) end = this->floors + 1;
	if (end == 0) end = this->floors;
	else --end;	
	if (end == this->floors) theEnd = true;

	if (begin == 0) {
		B.mant = this->mant;
		B.l_mant = this->l_mant;
	} else {
		B.mant = this->expo[begin - 1];
		B.l_mant = this->l_exp[begin - 1];
	}
	for (int8_t i = begin; i <= end; i++) {
		B.expo[i - begin] = this->expo[i];
		B.l_exp[i - begin] = this->l_exp[i];
	}
	B.floors = end - begin;
	if (!theEnd) B.expo[B.floors] /= pow(10, MAXPREC - B.l_exp[B.floors]);
	B.l_exp[B.floors] = 0;
	return B;
}

// Push a big number into a another big number at a given floor
// Fill in with 1s under the floor
void bigNumber::push (bigNumber & N, uint8_t floor) {
	// Serial.printf("push: floor = %d\n", floor);
	// Serial.printf("push: mant = %lld\n", N.mant);
	// for (byte i = 0; i <= N.floors; i++) Serial.printf("push: expo %d = %lld\n", i, N.expo[i]);

	if (floor + N.floors >= MAXFLOORS) {
		Serial.println("Push error : too many flooooors!");
		return;
	}
	double mant = 1.0f;
	double expo[MAXFLOORS] = {0};
	int floors;
	if (N.floors == 0) {
		if (N.expo[0] <= MAXPREC) {
			expo[0] = N.mant / pow(10.0f, MAXPREC - N.expo[0]);
			floors = 1;
		} else {
			expo[0] = N.mant / pow(10.0f, MAXPREC);
			expo[1] = N.expo[0];
			floors = 2;
		}
	} else {
		for (uint8_t i = 0; i < floor; i++) expo[i] = 1.0f;
		for (uint8_t i = 0; i <= N.floors; i++)
			expo[i + floor] = N.expo[i] / pow(10.0f, MAXPREC);

		if (floor != 0)
			expo[floor - 1] = N.mant  / pow(10.0f, MAXPREC);
		else 
			mant = N.mant / pow(10.0, MAXPREC);

		expo[floor + N.floors] = N.expo[N.floors];
		floors = floor + N.floors + 1;		
	}


	// Serial.printf("fin push: floors = %d\n", floors);
	// Serial.printf("fin push: mant = %f\n", mant);
	// for (byte i = 0; i <= floors; i++) Serial.printf("fin push: expo %d = %f\n", i, expo[i]);
	this->set(mant, expo, floors);
}

// Private method : display a given floor
void bigNumber::XXdisp (int64_t x, uint8_t prec, bool point) {
	char buff[20];
	uint8_t nDec = prec;
	sprintf(buff, "%lld", x);
	Serial.print(buff[0]);
	if (point) Serial.print('.');
	else nDec = 20;
	for (uint8_t i = 0; i < nDec; i++) {
		if (buff[i + 1] == 0) return;
		Serial.print(buff[i + 1]);
	}
}

// Display a bigNumber in a human understandable form (prec : number of figures)
void bigNumber::displayBigNumber (uint8_t prec) {
		if (this->mant != pow(10,MAXPREC)) XXdisp (this->mant, prec, true);
	for (uint8_t i = 0; i <= this->floors; i++) {
		if (i == this->floors) {
			Serial.print(" 10^");
			XXdisp(this->expo[i], prec, false);
		} else {
			Serial.print(" 10^(");
			if (this->expo[i] != pow(10,MAXPREC)) XXdisp(this->expo[i], prec, true);
		}
	}
	for (uint8_t i = 0; i <= this->floors - 1; i++) Serial.print(")");
	Serial.print(" ");
}

void bigNumber::displayStruct () {
	Serial.printf("%d floors\n", this->floors);
	char text [40];
	Serial.print ("mant:");
	XXdisp(this->mant, 16, true);
	Serial.println();
	// Serial.printf(" (e%d)\n", this->l_mant);

	for (uint8_t i = 0; i <= this->floors; i++) {
		Serial.printf("floor %d expo: ", i);
		XXdisp(this->expo[i], 16, (i != this->floors));
		Serial.println();
		// Serial.printf(" (e%d)\n", this->l_exp[i]);
	}
}

// Parse a big number from a char array
void bigNumber::parseNumber(char* input) {
	uint16_t index = 0;
	// find end of number
	uint16_t maxIndex = 0;
	while (input[maxIndex] != 0 && ((input[maxIndex] >= '0' && input[maxIndex] <= '9')
		|| input[maxIndex] == '.' || input[maxIndex] == 'e' || input[maxIndex] == 'E')) 
		++maxIndex;

	// count the 'e's
	uint8_t nE = 0;
	while (input[index] != 0 && index < maxIndex) {
		if (input[index] == 'e' or input[index] == 'E') ++nE;
		++ index;
	}

	// begin parsing the big number
	double mant = 0.0f;
	double expo[MAXFLOORS] = {0};
	index = 0;
	int k = 0;
	uint8_t index0 = 0;
	char buff[40];

	// remove unwanted characters
	while ((input[index] < '0' || input[index] > '9') && input[index] != '.') ++index;
	index0 = index;

	// parse mantissa (if any)
	while ((input[index] >= '0' && input[index] <= '9') || input[index] == '.')
		buff[index - index0] = input[index++];
	buff[index - index0] = 0;
	mant = atof(buff);
	expo[k] = 0.0;

	for (int i = 0; i < nE; i++) {
		// erase 'e'
		while (input[index] == 'e' || input[index] == 'E') {
			if (input[index + 1] == 'e' || input[index + 1] == 'E') {
				// case of several e following
				expo[k] = 1.0;
				++k;
				--nE;
			}
			++index;
		}
		index0 = index;

		// parse exponent (if any)
		while ((input[index] >= '0' && input[index] <= '9') || input[index] == '.')
			buff[index - index0] = input[index++];
		buff[index - index0] = 0;
		expo[k] = atof(buff);
		++k;
		if (k > MAXFLOORS) {
			Serial.println("Too many floors !!!");
			return;
		}
	}

  // Create big number form parsed data
	uint8_t floors = k;
	if (floors == 0) floors = 1;
	this->set(mant, expo, floors);

	// Remove parsed characters
	index0 = index;
	while (input[index] != 0) input[index - index0] = input[index++];
	input[index - index0] = 0;
}

// Compute log_10 (x!)
void bigNumber::bigStirling (bigNumber & N) {
	// Serial.printf("fact: floors = %d\n", N.floors);
	// Serial.printf("mant = %lld\n", N.mant);
	// for (byte i = 0; i <= N.floors; i++) Serial.printf("expo %d = %lld\n", i, N.expo[i]);

	if (N.floors == 0) { // Compute Stirling's approximation
		if (N.expo[0] < MAXPREC) {
			double x = N.mant / pow(10.0f, MAXPREC) * pow(10.0f, N.expo[0]);
			double fact = x * log(x) - x;
			double y = 8 * x * x * x + 4 * x * x + x + .033333333333333333;
			fact += log(y) / 6.0 + 0.5723649429247;  // log_e(PI)/2
			double log_10 = 2.3025850929940457;      // log_e(10)
			y = fact / log_10;

			// put it in a bigNumber
			uint64_t intPart = y;
			double fracPart = y - intPart;
			double mant = pow(10.0, fracPart);
			double expo[MAXFLOORS] = {0};
			expo[0] = intPart;
			int floors = 1;
			this->set(mant, expo, floors);
			return;
		} else { // Approximate N! by (N/e)^N
			bigNumber M = N.extract(0);
			M.mant /= 2.718281828459045235;
			M.bigPower(N);
			*this = M.extract(0);
			return;
		}
	}
	// Approximate N! by (N/e)^N
	bigNumber M = N.extract(0);
	M.mant /= 2.718281828459045235;
	M.bigPower(N);
	for (uint8_t i = 0; i < M.floors -1; i++) M.expo[i] = pow(10, MAXPREC);
	*this = M.extract(0);
}