void EEPROM_WriteString(char addr, String data)
{
    int _size = data.length();
    int i;
    for (i = 0; i < _size; i++)
    {
        EEPROM.write(addr + i, data[i]);
    }
    EEPROM.write(addr + _size, '\0'); //Add termination null character for String Data
    delay(1);
    EEPROM.commit();
    delay(1);
}
String EEPROM_ReadString(char addr)
{
    int i;
    char data[100]; //Max 100 Bytes
    int len = 0;
    unsigned char k;
    k = EEPROM.read(addr);
    while (k != '\0' && len < 99) //Read until null character
    {
        k = EEPROM.read(addr + len);
        data[len] = k;
        len++;
    }
    data[len] = '\0';
    return String(data);
}
void EEPROM_WriteUInt(char address, unsigned int number)
{
    EEPROM.write(address, number >> 8);
    EEPROM.write(address + 1, number & 0xFF);
    delay(1);
    EEPROM.commit();
    delay(1);
}
unsigned int EEPROM_ReadUInt(char address)
{
    return (EEPROM.read(address) << 8) + EEPROM.read(address + 1);
}
