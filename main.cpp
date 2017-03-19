#include <iostream>
#include <stdlib.h>
#include <fstream>
#include <cstring>
#include <vector>
#include <string.h>
using namespace std;
int RNN;
void checkRNN(fstream &File)
{

    /*This function checks if its the first time to open the file
    and if so it initializes the RNN to -1 by writing it to file */
    File.seekg(0,ios::end); ///Checks the end of file, if tellg returns 0, it will be true.
    if(!File.tellg())
    {
        RNN=-1;
        File.write((char*)&RNN,sizeof(RNN));
    }
    else
    {
        File.seekg(0,ios::beg);
        File.read((char*)&RNN,sizeof(RNN));
    }
}
void UpdateRNN(fstream &File){
    File.seekp(0,ios::beg);
    File.write((char*)&RNN,sizeof(RNN));
}

class electronic
{
public:
    char ID[21];
    char  Name[31];
    char Brand[31];
    int x;
    int length;
    float Price;

    friend istream& operator>> (istream&in,electronic& A)
    {
        A.x=-2;
        cout<<"Please enter the ID"<<endl;
        in >> A.ID;
        in.ignore();
        cout<<"Please enter the name of device"<<endl;
        in.getline(A.Name,31);
        cout<<"Please enter the brand of device"<<endl;
        in.getline(A.Brand,31);
        cout<<"Please enter the price"<<endl;
        in>> A.Price;
        A.length= strlen(A.ID)+strlen(A.Name)+strlen(A.Brand)+sizeof(A.Price)+8;
        return in ;
    }
    void Adddeleted(fstream &file)
    {
        x=-2;
        file.write((char*)&x,sizeof(x));
        file.write(ID,strlen(ID));
        file.write("#",1);
        file.write(Name,strlen(Name));
        file.write("#",1);
        file.write(Brand,strlen(Brand));
        file.write("#",1);
        file.write((char*)&Price,sizeof(Price));
        file.write("#",1);
    }
    void AddToFile(fstream &file)
    {
        file.seekp(0,ios::end);
        x=-2;
        file.write((char*)&length,sizeof(length));
        file.write((char*)&x,sizeof(x));
        file.write(ID,strlen(ID));
        file.write("#",1);
        file.write(Name,strlen(Name));
        file.write("#",1);
        file.write(Brand,strlen(Brand));
        file.write("#",1);
        file.write((char*)&Price,sizeof(Price));
        file.write("#",1);
    }
    void ReadFromFile(fstream& file)
    {
        file.read((char*)&length,sizeof(length));
        int old=file.tellg();
        file.read((char*)&x,sizeof(x));

        file.getline(ID,21,'#');

        file.getline(Name,31,'#');

        file.getline(Brand,31,'#');

        file.read((char*)&Price,sizeof(Price));
        file.seekg(length-file.tellg()+old,ios::cur);
        return ;

    }
    friend ostream& operator <<(ostream& out, electronic &Device)
    {
        if (Device.x==-3) out << "Item not found.";
        else
        {
            out<<endl;
            out<<"Device ID: ";

            out<<Device.ID<<endl;

            out<<"Device Name: ";
            out<<Device.Name<<endl;

            out<<"Device Brand: ";
            out<<Device.Brand<<endl;

            out<<"Device Price: ";
            out<<Device.Price<<endl;
        }
        return out;

    }
};

electronic Search(fstream &File,char ID[])
{
    File.seekg(0,ios::end);
    int lastByte = File.tellg();
    File.seekg(4, ios::beg);
    electronic B;
    while (File.tellg() != lastByte)
    {
        electronic A;
        A.ReadFromFile(File);
        if(strcmp(A.ID,ID)==0&& A.x==-2)
        {
            return A;
        }
        B=A;
    }
    B.x=-3;
    cout<<"There is no such record!"<<endl;
    return B;
}
void Add(fstream &File,electronic ToBeAdded)
{
    int Size,Pre,TempRNN=RNN;
    while(TempRNN!=-1)
    {
        File.seekg(TempRNN,ios::beg);
        File.read((char*)&Size,sizeof(Size));
        File.read((char*)&Pre,sizeof(Pre));
        File.seekg(-4,ios::cur);
        if(Size>=ToBeAdded.length)
        {
            ToBeAdded.Adddeleted(File);
            RNN=Pre;
            UpdateRNN(File);
            return;
        }
        TempRNN=Pre;
    }
    ToBeAdded.AddToFile(File);
}
void Delete (fstream &File, char ID[])
{
    electronic A= Search(File,ID);
    if(A.x==-3)
    {
        return;
    }
    else
    {
        A.x=RNN;
        File.seekp(-A.length,ios::cur);
        File.write((char*)&A.x,sizeof(A.x));
        RNN=File.tellp()-8;
        UpdateRNN(File);
    }
}

void Update(fstream&File,char ID[])
{
    electronic A= Search(File,ID);
    if(A.x==-3)
    {
        return;
    }
    electronic B;
    cin >> B;
    if (A.length>=B.length)
    {
        File.seekp(-A.length,ios::cur);
        B.Adddeleted(File);
    }
    else
    {
        Delete(File,A.ID);
        Add(File,B);
    }

}
void PrintAll(fstream& File)
{
    File.seekg(0,ios::end);
    int lastByte = File.tellg();
    File.seekg(4, ios::beg);
    while (File.tellg() != lastByte)
    {
        electronic A;
        A.ReadFromFile(File);
        if(A.x==-2)
            cout <<A;
    }
}
void CompactFile(fstream &File)
{
    vector <electronic> Devices;
    File.seekg(0,ios::end);
    int lastByte = File.tellg();
    File.seekg(4, ios::beg);
    while (File.tellg() != lastByte)
    {
        electronic A;
        A.ReadFromFile(File);
        A.length=strlen(A.ID)+strlen(A.Name)+strlen(A.Brand)+sizeof(A.Price)+8;
        if(A.x==-2)
            Devices.push_back(A);
    }
    File.close();
    File.open("Database.txt",ios::out| ios::trunc);
    checkRNN(File);
    File.seekp(4, ios::beg);
    for(int i=0; i<Devices.size(); i++)
    {
        Devices[i].AddToFile(File);
    }
    File.seekg(0,ios::end);
    cout<<"Bytes before: "<<lastByte<<endl;
    cout<<"Bytes after: "<<File.tellg()<<endl;

    cout<<"Bytes free: "<<lastByte-File.tellg();

    File.close();


    File.open("Datbase.txt",ios::in|ios::out);

}

void Mainmenu(fstream &file)
{
    checkRNN(file);
    cout <<"\t\t\tWelcome to the store management system"<<endl;
    int Choice;
    char ID[21];
    cout<<"1.Add device"<<endl;
    cout<<"2.Update device"<<endl;
    cout<<"3.Delete device"<<endl;
    cout<<"4.Search for device"<<endl;
    cout<<"5.Compact file"<<endl;
    cout<<"6.Print all devices"<<endl;
    cout<<"7.Clear screen"<<endl;
    cout<<"8.Exit"<<endl;
        cin >>Choice;


switch (Choice){
case 1:
    electronic A;
    cin >> A;
Add(file,A);
    break;
case 2:
    cout<<"ID of record to update"<<endl;
    cin >>ID;
    Update(file,ID);
    break;
case 3:
    cout<<"Please enter the ID"<<endl;
    cin>>ID;
    Delete(file,ID);
    break;
case 4:
    cout<<"Please enter the ID"<<endl;
    cin >>ID;
    Search(file,ID);
    break;
case 5:
    CompactFile(file);
    break;
case 6:
    PrintAll(file);
    break;
case 7:
    system("CLS");
    break;
case 8:
    return;
    }
    Mainmenu(file);

};



int main()
{
    fstream file("Database.txt",ios::in|ios::out);
    Mainmenu(file);
    file.close();
    return 0;
}
