

void LEDON(void);
void LEDOFF(void);
void LOOP(void);



// Assembly data 
extern const int ITERATIONS;


int altmain(){

    int j = 4; /* (int) ITERATIONS; */
    while(j--){
        LEDON();
        LOOP();
        LEDOFF();
        LOOP();
    }

    return 0;
}