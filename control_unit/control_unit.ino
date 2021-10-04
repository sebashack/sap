#define LDA    32
#define ADD    33
#define SUB    34
#define MUL    35
#define DIV    36
#define MOD    37
#define INC    38
#define DECM   39
#define OP1    40
#define OP2    41
#define OP3    42
#define RETURN 43
#define OUT    44
#define HLT    45
#define CALL   46
#define GOTO   47

#define DE A15
#define LO A14
#define CP A13
#define EP A12
#define LM A11
#define ER A10
#define LI A9
#define EI A8

#define LA 21
#define EA 20
#define AD 19
#define SU 18
#define MU 17
#define DI 16
#define EU 15
#define LB 14
#define IN 52

#define CI0 48
#define CI1 49
#define CI2 50
#define CI3 51

#define LOAD 3
#define LD0  4
#define LD1  5
#define LD2  6
#define LD3  7

#define ADDR0 8
#define ADDR1 9
#define ADDR2 10
#define ADDR3 11


#define CLK      53
#define CLK_FREQ 10

#define HN 30
#define TN 31
#define UN 12

void setup()
{
    Serial.begin(9600);

    DDRF = 0B11111111;

    pinMode(LDA, INPUT);
    pinMode(ADD, INPUT);
    pinMode(SUB, INPUT);
    pinMode(MUL, INPUT);
    pinMode(DIV, INPUT);
    pinMode(MOD, INPUT);
    pinMode(INC, INPUT);
    pinMode(DECM, INPUT);
    pinMode(OP1, INPUT);
    pinMode(OP2, INPUT);
    pinMode(OP3, INPUT);
    pinMode(RETURN, INPUT);
    pinMode(OUT, INPUT);
    pinMode(HLT, INPUT);
    pinMode(CALL, INPUT);
    pinMode(GOTO, INPUT);

    pinMode(CP, OUTPUT);
    pinMode(EP, OUTPUT);
    pinMode(LM, OUTPUT);
    pinMode(ER, OUTPUT);
    pinMode(LI, OUTPUT);
    pinMode(EI, OUTPUT);
    pinMode(LA, OUTPUT);
    pinMode(EA, OUTPUT);
    pinMode(AD, OUTPUT);
    pinMode(SU, OUTPUT);
    pinMode(MU, OUTPUT);
    pinMode(DI, OUTPUT);
    pinMode(EU, OUTPUT);
    pinMode(LB, OUTPUT);
    pinMode(LO, OUTPUT);

    pinMode(IN, OUTPUT);
    pinMode(DE, OUTPUT);

    pinMode(CLK, INPUT);

    pinMode(CI0, INPUT);
    pinMode(CI1, INPUT);
    pinMode(CI2, INPUT);
    pinMode(CI3, INPUT);

    pinMode(LOAD, OUTPUT);
    pinMode(LD0, OUTPUT);
    pinMode(LD1, OUTPUT);
    pinMode(LD2, OUTPUT);
    pinMode(LD3, OUTPUT);

    pinMode(ADDR0, INPUT);
    pinMode(ADDR1, INPUT);
    pinMode(ADDR2, INPUT);
    pinMode(ADDR3, INPUT);

    DDRA = 0B00000000;
}

byte disp[10] = {63, 6, 91, 79, 102, 109, 125, 7, 127, 103};

byte returnAddr[4] = {0,0,0,0};

byte ci[4] = {0,0,0,0};
void readCurrentIns()
{
    ci[0] = digitalRead(CI0);
    ci[1] = digitalRead(CI1);
    ci[2] = digitalRead(CI2);
    ci[3] = digitalRead(CI3);
}

byte jumpAddr[4] = {0,0,0,0};
void readJumpAddr()
{
    jumpAddr[0] = digitalRead(ADDR0);
    jumpAddr[1] = digitalRead(ADDR1);
    jumpAddr[2] = digitalRead(ADDR2);
    jumpAddr[3] = digitalRead(ADDR3);
}

void loadInstruction(byte b0, byte b1, byte b2, byte b3)
{
    Serial.println("loading instruction");

    while(true)
    {
        if(!digitalRead(CLK))
        {
            digitalWrite(LOAD, 1);
            digitalWrite(LD0, b0);
            digitalWrite(LD1, b1);
            digitalWrite(LD2, b2);
            digitalWrite(LD3, b3);
            delay(CLK_FREQ);
            digitalWrite(LOAD, 0);
            digitalWrite(LD0, 0);
            digitalWrite(LD1, 0);
            digitalWrite(LD2, 0);
            digitalWrite(LD3, 0);
            break;
        }
    }
}

void dispatchSwitches(byte pin0, byte pin1)
{
    while(true)
    {
        if(!digitalRead(CLK))
        {
            digitalWrite(pin0, 1);
            digitalWrite(pin1, 1);
            delay(CLK_FREQ);
            digitalWrite(pin1, 0);
            digitalWrite(pin0, 0);

            break;
        }
    }
}

void simpleArithOp(byte pin)
{
    dispatchSwitches(EI, LM);
    dispatchSwitches(ER, LB);
    digitalWrite(pin, 1);
    digitalWrite(pin, 0);
    dispatchSwitches(EU, LA);
}

void setDisplay(byte d0, byte v0, byte d1, byte v1, byte d2, byte v2)
{
    digitalWrite(d0, v0);
    digitalWrite(d1, v1);
    digitalWrite(d2, v2);
}

void show(byte n)
{
    unsigned int h = n / 100;
    unsigned int r = n % 100;
    unsigned int t = r / 10;
    unsigned int u = r % 10;

    setDisplay(TN, 0, UN, 0, HN, 1);
    PORTF = disp[h];
    delay(1);

    setDisplay(HN, 0, UN, 0, TN, 1);
    PORTF = disp[t];
    delay(1);

    setDisplay(TN, 0, HN, 0, UN, 1);
    PORTF = disp[u];
    delay(1);
}

void cp()
{
    while(true)
    {
        if(!digitalRead(CLK))
        {
            digitalWrite(CP, 1);
            delay(CLK_FREQ);
            digitalWrite(CP, 0);
            break;
        }
    }
}

void increment()
{
    digitalWrite(IN, 1);
    digitalWrite(AD, 1);
    dispatchSwitches(EU, LA);
    digitalWrite(AD, 0);
    digitalWrite(IN, 0);
}

void decrement()
{
    digitalWrite(DE, 1);
    digitalWrite(SU, 1);
    dispatchSwitches(EU, LA);
    digitalWrite(SU, 0);
    digitalWrite(DE, 0);
}

void resetAccum()
{
    dispatchSwitches(EA, LB);
    digitalWrite(SU, 1);
    digitalWrite(SU, 0);
    dispatchSwitches(EU, LA);
}


void loop()
{
    dispatchSwitches(EP, LM);
    dispatchSwitches(ER, LI);

    if (digitalRead(LDA))
    {
        Serial.println("INPUT: LDA");
        dispatchSwitches(EI, LM);
        dispatchSwitches(ER, LA);

        cp();
    }

    if (digitalRead(ADD))
    {
        Serial.println("INPUT: ADD");
        simpleArithOp(AD);
        cp();
    }

    if (digitalRead(SUB))
    {
        Serial.println("INPUT: SUB");
        simpleArithOp(SU);
        cp();
    }

    if (digitalRead(MUL))
    {
        Serial.println("INPUT: MUL");
        simpleArithOp(MU);
        cp();
    }

    if (digitalRead(DIV))
    {
        Serial.println("INPUT: DIV");
        dispatchSwitches(EI, LM);
        dispatchSwitches(ER, LB);

        byte n = PINA;

        if (n == 0)
        {
            Serial.println("Division by 0.");
            while(true)
            {
                setDisplay(TN, 0, UN, 0, HN, 1);
                PORTF = 121;
                delay(1);

                setDisplay(HN, 0, UN, 0, TN, 1);
                PORTF = 121;
                delay(1);

                setDisplay(TN, 0, HN, 0, UN, 1);
                PORTF = 62;
                delay(1);
            }
        }
        else
        {
            digitalWrite(DI, 1);
            digitalWrite(DI, 0);
            dispatchSwitches(EU, LA);
        }

        cp();
    }

    if (digitalRead(MOD))
    {
        Serial.println("INPUT: MOD");
        dispatchSwitches(EI, LM);
        dispatchSwitches(ER, LB);

        byte b = PINA;
        dispatchSwitches(EA, LB);
        byte a = PINA;

        resetAccum();

        // Reestablish B Register
        for (byte i = b; i > 0; i--)
        {
            increment();
        }

        dispatchSwitches(EA, LB);

        for (byte i = b; i > 0; i--)
        {
            decrement();
        }

        // Reestablish Accum
        for (byte i = a; i > 0; i--)
        {
            increment();
        }

        simpleArithOp(DI);

        digitalWrite(MU, 1);
        digitalWrite(MU, 0);
        dispatchSwitches(EU, LA);
        dispatchSwitches(EA, LB);

        byte result = PINA;

        for (byte i = result; i > 0; i--)
        {
            decrement();
        }

        // Reestablish Accum
        for (byte i = a; i > 0; i--)
        {
            increment();
        }

        digitalWrite(SU, 1);
        digitalWrite(SU, 0);
        dispatchSwitches(EU, LA);

        cp();
    }

    if (digitalRead(INC))
    {
        Serial.println("INPUT: INC");
        increment();
        cp();
    }

    if (digitalRead(DECM))
    {
        Serial.println("INPUT: DECM");
        decrement();
        cp();
    }

    if (digitalRead(OP1))
    {
        Serial.println("INPUT: OP1 (POWER)");
        dispatchSwitches(EI, LM);
        dispatchSwitches(ER, LB);

        byte exponent = PINA;

        if (exponent > 0)
        {
            dispatchSwitches(EA, LB);

            for (byte i = exponent; i > 1; i--)
            {
                digitalWrite(MU, 1);
                digitalWrite(MU, 0);
                dispatchSwitches(EU, LA);
            }
        }
        else
        {
            digitalWrite(MU, 1);
            digitalWrite(MU, 0);
            dispatchSwitches(EU, LA);
            increment();
        }

        cp();
    }

    if (digitalRead(OP2))
    {
        Serial.println("INPUT: OP2 (FACTORIAL)");
        readCurrentIns();
        dispatchSwitches(EA, LB);

        byte n = PINA;

        if (n > 2)
        {
            byte i = 1;
            while (i < n - 1)
            {
                for (byte j = i; j > 0; j--)
                {
                    decrement();
                }

                digitalWrite(MU, 1);
                digitalWrite(MU, 0);
                dispatchSwitches(EU, LA);
                dispatchSwitches(EA, LB);

                i++;

                if (i < n - 1)
                {
                    byte result = PINA;

                    for (byte j = result; j > 0; j--)
                    {
                        decrement();
                    }

                    for (byte j = n; j > 0; j--)
                    {
                        increment();
                    }
                }
            }

            loadInstruction(ci[0], ci[1], ci[2], ci[3]);
        }

        if (n == 0)
        {
            increment();
        }


        cp();
    }

    if (digitalRead(OP3))
    {
        Serial.println("INPUT: OP3 (SQRT)");
        readCurrentIns();
        dispatchSwitches(EA, LB);

        byte n = PINA;

        if (n > 1)
        {
            resetAccum();

            increment();
            dispatchSwitches(EA, LB);

            byte result = PINA;

            byte i;
            while(result <= n)
            {
                increment();
                dispatchSwitches(EA, LB);

                i = PINA;

                digitalWrite(MU, 1);
                digitalWrite(MU, 0);
                dispatchSwitches(EU, LA);
                dispatchSwitches(EA, LB);

                result = PINA;
                resetAccum();

                for (byte j = i; j >0; j--)
                {
                    increment();
                }

                dispatchSwitches(EA, LB);
            }

            decrement();
        }

        cp();
    }

    if (digitalRead(RETURN))
    {
        Serial.println("INPUT: RETURN");
        loadInstruction(returnAddr[0], returnAddr[1], returnAddr[2], returnAddr[3]);
        cp();
    }

    if (digitalRead(OUT))
    {
        Serial.println("INPUT: OUT");
        dispatchSwitches(EA, LB);
        byte result = PINA;

        for (int i = 0; i < 550; i++)
        {
            show(result);
        }

        digitalWrite(HN, 0);
        digitalWrite(UN, 0);
        digitalWrite(TN, 0);

        cp();
    }

    if (digitalRead(HLT))
    {
        Serial.println("INPUT: HLT");
        while (true)
        {
            delay(1000);
            Serial.println("Program halted...");
        }
    }

    if (digitalRead(CALL))
    {
        Serial.println("INPUT: CALL");

        readCurrentIns();
        returnAddr[0] = ci[0];
        returnAddr[1] = ci[1];
        returnAddr[2] = ci[2];
        returnAddr[3] = ci[3];

        readJumpAddr();
        loadInstruction(jumpAddr[0], jumpAddr[1], jumpAddr[2], jumpAddr[3]);
    }

    if (digitalRead(GOTO))
    {
        Serial.println("INPUT: GOTO");

        readJumpAddr();
        loadInstruction(jumpAddr[0], jumpAddr[1], jumpAddr[2], jumpAddr[3]);
    }
}
