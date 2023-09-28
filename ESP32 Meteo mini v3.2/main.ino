void setup()
{
    Serial.begin(9600);
}

void loop()
{
    int counter = 0;
    while (1)
    {
        Serial.print("Hodnota proměnné je: ");
        Serial.println(counter++);
        delay(500);
    }
}