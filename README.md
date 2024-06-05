# Assignment 1 for "Advanced Programming"


In results folder there are screenshots and results of the commands we did .

## Get Started


```bash
sudo apt-get install libreadline-dev

make all

./myshell
           ───▄▀▀▀▄▄▄▄▄▄▄▀▀▀▄───
            ───█▒▒░░░░░░░░░▒▒█───
            ────█░░█░░░░░█░░█────
            ─▄▄──█░░░▀█▀░░░█──▄▄─
            █░░█─▀▄░░░░░░░▄▀─█░░█
            █▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀█
            █░░╦─╦╔╗╦─╔╗╔╗╔╦╗╔╗░░█
            █░░║║║╠─║─║─║║║║║╠─░░█
            █░░╚╩╝╚╝╚╝╚╝╚╝╩─╩╚╝░░█
            █▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄█
hello: date >> myfile
hello: cat myfile
Wed 05 Jun 2024 03:29:50 PM IDT
hello: date -u >> myfile
hello: cat myfile
Wed 05 Jun 2024 03:29:50 PM IDT
Wed 05 Jun 2024 12:30:11 PM UTC
hello: wc -l < myfile
2
hello: prompt = hi:
hi: mkdir mydir
hi: cd mydir
hi: pwd
/home/orel/Documents/AP_Task1/mydir
hi: touch file1touch file1 file2 file3
hi: ls 
file1  file2  file3
hi: !!
file1  file2  file3
hi: echo abc xyz
abc xyz
hi: ls
file1  file2  file3
hi: echo $?
0
hi: ls no_such_file
ls: cannot access 'no_such_file': No such file or directory
hi: echo $?
2
hi: ls no_such_ls no_such_file 2> file
hi: ^C
You typed Control-C!
hi: cat > colors.txt 
blue 
black 
red
red
green
blue
green
red
red
blue
hi: cat colors.txt
blue 
black 
red
red
green
blue
green
red
red
blue
hi: cat colors.txt | cat | cat | cat 
blue 
black 
red
red
green
blue
green
red
red
blue
hi: sort colorssort colors.txt | uniq –c | sort –r | head -3
uniq: –c: No such file or directory
sort: cannot read: –r: No such file or directory
hi: quit

```

## Authors

- Orel Dayan
- Yuval Baror


