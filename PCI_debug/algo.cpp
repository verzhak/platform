#include "algo.h"

Algo::Algo()
{
}


void Algo::read(const QString filename)
{
    file.setFileName(filename);
    if (!file.open(QIODevice::ReadWrite))
    {
        printf("Ошибка при открытии файла\n");
    }
    data_reg= (uint32_t *) file.map (0,size);
   while(true)
{
       while(data_reg[REG_STATE] != STATE_WAIT)
       {
           sched_yield();
       }
    data_reg[REG_STATE] = STATE_WRITE;
    cout<< "Новый кадр"<<endl;
    int count_matr=data_reg[REG_NUMBER_OF_MATRICES];
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
    cout << "Количество видовых матриц " << count_matr << "\n";
    int number_matr=1;
    int a=1; int b=count_element_matr+1;
        for (int i=0;i<map_contour.size();i++)
        {
            while (b <= (matrix_size/4)+1)
            {
            cout<< "Матрица "<<number_matr<<endl;
            for (int k=a; k<b;k++)
             {
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
        }
    map_contour.clear();

    unsigned number_of_result;
    if (count_matr!=0)
    {
       number_of_result=rand()% count_matr;
    }
    else
    {
      number_of_result=0;
    }
    data_reg[REG_NUMBER_OF_RESULTS]=number_of_result;
    int count_result=0;
    float array_result[6] = { 1, 2, 3, 4, 5, 6};
    //uint32_t array_result[6] = { 0xAAAAAAAA,0xBBBBBBBB, 0xCCCCCCCC, M_PI, M_E, M_LN10};
    int j=1;
    while (count_result<data_reg[REG_NUMBER_OF_RESULTS])
    {
        while((data_reg[REG_STATE] != STATE_WAIT) &&(data_reg[REG_STATE] != STATE_WRITE_END))
        {
            sched_yield();
        }
        for (int i=0;i<496;i++)
        {
            data[i]=array_result[j-1];
            if (j<6) {j++;} else {j=1;}
            if ((j % 6)==0) {count_result+=1;}
        }

        data_reg[REG_STATE]=STATE_READ;
     }
    while(data_reg[REG_STATE] != STATE_WAIT)
    {
        sched_yield();
    }
     data_reg[REG_STATE]=STATE_READ_END;
   }
}


