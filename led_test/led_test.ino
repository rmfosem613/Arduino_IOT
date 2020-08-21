void setup() {
  // put your setup code here, to run once:
  pinMode(23,OUTPUT);
  Serial.begin(115200);
}
int i=0;
void loop() {
  // put your main code here, to run repeatedly:
  int j=0;
  static int k=0;
  digitalWrite(23,1);
  delay(1000);
  digitalWrite(23,0);
  delay(1000);
  Serial.printf("i=%d, j=%d\n", i++,j++);

}
