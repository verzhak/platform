#include "algo.h"

Algo::Algo()
{
}


void Algo::read(const string fileName)
{

    file = open(fileName.c_str(), O_RDWR | O_SYNC ) ;
    if (file == -1)
        perror ("Ошибка при открытии файла");

   data_reg = (uint32_t *) mmap(NULL, size, PROT_READ | PROT_WRITE, MAP_SHARED , file, 0);
   //int cadr=0;
   while(true)
{
    data_reg[REG_STATE] = STATE_WRITE;
    int count_matr=data_reg[REG_NUMBER_OF_MATRICES];
   // cout << "Количество видовых матриц " << count_matr << "\n";
    int count_contour=data_reg[REG_NUMBER_OF_CONTOUR];
    //cout << "Количество контуров " << count_contour << "\n";

    while (data_reg[REG_STATE] != STATE_WRITE_END)
    {

        while((data_reg[REG_STATE] != STATE_WAIT)&&(data_reg[REG_STATE] != STATE_WRITE_END))
        {
            sched_yield();
        }

        if (data_reg[REG_STATE] == STATE_WRITE_END)
         {
            break;
         }


        data=data_reg+offset_reg;
        unsigned buf_size=contour_size+matrix_size;
        shared_ptr<uint32_t> buf;
        buf.reset(new uint32_t[buf_size],std::default_delete<uint32_t[]>());
        uint32_t * ptr_buf =buf.get();
       //unique_ptr<uint32_t[]> buf (new uint32_t[buf_size]);
        //uint32_t* buf= new uint32_t[buf_size];

        memcpy (ptr_buf,data,buf_size);

        map_contour.push_back(buf);

        data_reg[REG_STATE] = STATE_WRITE;

     }
    //Вывод контуров
    int number_contour=1;
    cout<< "Контур "<<number_contour<<endl;
    for (int i=0;i<map_contour.size();i++)
    {

        for (int k=0; k<(contour_size/4);k++)
        {

            if (*(map_contour[i].get()+k)!= 0xFFFFFFFF)
            {
               cout<<*(float*)(map_contour[i].get()+k)<<endl;
            }
            else
            {
                cout<<endl;
                number_contour++;
                cout<< "Контур "<<number_contour<<endl;
            }
        }

    }
    //Вывод матриц
   /* int number_matr=1;
    int a=1; int b=count_element_matr+1;
    for (int i=0;i<map_contour.size();i++)
    {

        while (b <= (matrix_size/4)+1)
        {
        cout<< "Матрица "<<number_matr<<endl;
        for (int k=a; k<b;k++)
        {
            //cout<<k<<endl;
            cout<<*(float*)(map_contour[i].get()+offset_matr+k-1)<<" ";
            if ((k % 4)==0)
            {
                cout<<endl;
            }
        }
            cout<<endl;
            number_matr++;


            a=a+count_element_matr;b=b+count_element_matr;
       }
       a=1;b=count_element_matr+1;
    }*/

    /*for (auto buf:map_contour)
    {
        delete []buf;
    }*/
    map_contour.clear();
   }

}


