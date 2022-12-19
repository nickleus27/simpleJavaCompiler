import unittest
from subprocess import run
from os import remove
from os import path

class TestStringMethods(unittest.TestCase):

    def test_test0(self):
        exitStatus = run(["../build/./sjc", "test0.sjava"]).returncode
        self.assertEqual(exitStatus, 0)
        if path.isfile("test0.sjava.out"):
            with open('test0.txt', "w") as outfile:
                run("./test0.sjava.out", stdout=outfile)
                outfile.close()
        if path.isfile("test0.sjava.o"):
            remove("test0.sjava.o")
        if path.isfile("test0.sjava.out"):
            remove("test0.sjava.out")
        if path.isfile("test0.sjava.s"):
            remove("test0.sjava.s")

    def test_test1(self):
        exitStatus = run(["../build/./sjc", "test1.sjava"]).returncode
        self.assertEqual(exitStatus, 0)
        if path.isfile("test1.sjava.out"):
            with open('test1.txt', "w") as outfile:
                run("./test1.sjava.out", stdout=outfile)
                outfile.close()
        if path.isfile("test1.sjava.o"):
            remove("test1.sjava.o")
        if path.isfile("test1.sjava.out"):
            remove("test1.sjava.out")
        if path.isfile("test1.sjava.s"):
            remove("test1.sjava.s")

    def test_test2(self):
        exitStatus = run(["../build/./sjc", "test2.sjava"]).returncode
        self.assertEqual(exitStatus, 0)
        if path.isfile("test2.sjava.out"):
            with open('test2.txt', "w") as outfile:
                run("./test2.sjava.out", stdout=outfile)
                outfile.close()
        if path.isfile("test2.sjava.o"):
            remove("test2.sjava.o")
        if path.isfile("test2.sjava.out"):
            remove("test2.sjava.out")
        if path.isfile("test2.sjava.s"):
            remove("test2.sjava.s")

    def test_test2_1(self):
        exitStatus = run(["../build/./sjc", "test2-1.sjava"]).returncode
        self.assertEqual(exitStatus, 0)
        if path.isfile("test2-1.sjava.out"):
            with open('test2-1.txt', "w") as outfile:
                run("./test2-1.sjava.out", stdout=outfile)
                outfile.close()
        if path.isfile("test2-1.sjava.o"):
            remove("test2-1.sjava.o")
        if path.isfile("test2-1.sjava.out"):
            remove("test2-1.sjava.out")
        if path.isfile("test2-1.sjava.s"):
            remove("test2-1.sjava.s")

    def test_test3(self):
        exitStatus = run(["../build/./sjc", "test3.sjava"]).returncode
        self.assertEqual(exitStatus, 0)
        if path.isfile("test3.sjava.out"):
            with open('test3.txt', "w") as outfile:
                run("./test3.sjava.out", stdout=outfile)
                outfile.close()
        if path.isfile("test3.sjava.o"):
            remove("test3.sjava.o")
        if path.isfile("test3.sjava.out"):
            remove("test3.sjava.out")
        if path.isfile("test3.sjava.s"):
            remove("test3.sjava.s")

    def test_test4(self):
        exitStatus = run(["../build/./sjc", "test4.sjava"]).returncode
        self.assertEqual(exitStatus, 0)
        if path.isfile("test4.sjava.out"):
            with open('test4.txt', "w") as outfile:
                run("./test4.sjava.out", stdout=outfile)
                outfile.close()
        if path.isfile("test4.sjava.o"):
            remove("test4.sjava.o")
        if path.isfile("test4.sjava.out"):
            remove("test4.sjava.out")
        if path.isfile("test4.sjava.s"):
            remove("test4.sjava.s")

    def test_test4_1(self):
        exitStatus = run(["../build/./sjc", "test4-1.sjava"]).returncode
        self.assertEqual(exitStatus, 0)
        if path.isfile("test4-1.sjava.out"):
            with open('test4-1.txt', "w") as outfile:
                run("./test4-1.sjava.out", stdout=outfile)
                outfile.close()
        if path.isfile("test4-1.sjava.o"):
            remove("test4-1.sjava.o")
        if path.isfile("test4-1.sjava.out"):
            remove("test4-1.sjava.out")
        if path.isfile("test4-1.sjava.s"):
            remove("test4-1.sjava.s")

    def test_test5(self):
        exitStatus = run(["../build/./sjc", "test5.sjava"]).returncode
        self.assertEqual(exitStatus, 0)
        if path.isfile("test5.sjava.out"):
            with open('test5.txt', "w") as outfile:
                run("./test5.sjava.out", stdout=outfile)
                outfile.close()
        if path.isfile("test5.sjava.o"):
            remove("test5.sjava.o")
        if path.isfile("test5.sjava.out"):
            remove("test5.sjava.out")
        if path.isfile("test5.sjava.s"):
            remove("test5.sjava.s")

    def test_test6(self):
        if path.isfile("test6.sjava"):
            exitStatus = run(["../build/./sjc", "test6.sjava"]).returncode
            self.assertEqual(exitStatus, 0)
            if path.isfile("test6.sjava.out"):
                with open('test6.txt', "w") as outfile:
                    run("./test6.sjava.out", stdout=outfile)
                    outfile.close()
            if path.isfile("test6.sjava.o"):
                remove("test6.sjava.o")
            if path.isfile("test6.sjava.out"):
                remove("test6.sjava.out")
            if path.isfile("test6.sjava.s"):
                remove("test6.sjava.s")

    def test_test7(self):
        exitStatus = run(["../build/./sjc", "test7.sjava"]).returncode
        self.assertEqual(exitStatus, 0)
        if path.isfile("test7.sjava.out"):
            with open('test7.txt', "w") as outfile:
                run("./test7.sjava.out", stdout=outfile)
                outfile.close()
        if path.isfile("test7.sjava.o"):
            remove("test7.sjava.o")
        if path.isfile("test7.sjava.out"):
            remove("test7.sjava.out")
        if path.isfile("test7.sjava.s"):
            remove("test7.sjava.s")

    def test_test8(self):
        if path.isfile("test8.sjava"):
            exitStatus = run(["../build/./sjc", "test8.sjava"]).returncode
            self.assertEqual(exitStatus, 0)
            if path.isfile("test8.sjava.out"):
                with open('test8.txt', "w") as outfile:
                    run("./test8.sjava.out", stdout=outfile)
                    outfile.close()
            if path.isfile("test8.sjava.o"):
                remove("test8.sjava.o")
            if path.isfile("test8.sjava.out"):
                remove("test8.sjava.out")
            if path.isfile("test8.sjava.s"):
                remove("test8.sjava.s")
if __name__ == '__main__':
    unittest.main()