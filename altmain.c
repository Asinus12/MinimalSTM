

void LEDON(void);
void LEDOFF(void);
void LOOP(void);



// Assembly data 
extern const int ITERATIONS;


int altmain(){

    int j = 3; /* (int) ITERATIONS; */
    while(j--){
        LEDON();
        LOOP();
        LEDOFF();
        LOOP();
    }

    return 0;
}