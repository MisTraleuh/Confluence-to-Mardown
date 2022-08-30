# 🌳 Generation tree

## ⚡ Installation | Prerequisites

* [Gcc](https://gcc.gnu.org) (version >= 9.4.0)
* [Node.js](https://nodejs.org) (version >= 18.4.0)

## ⁉️ Export the documentation

Export the documentation on **HTML** and unzip it in the **docs folder**.

## 🏁 Start | Configuration

```bash
$ cd gen_tree
$ gcc main.c -o gen_tree && npm i
$ ./gen_tree <PathOfSpace> <PathOfResult>
```

## 🎁 Example of use

```bash
$ git clone https://github.com/CatalanNathan/Confluence-to-Mardown
$ cd documentation-ubika/gen_tree
$ gcc main.c -o gen_tree && npm i
$ ./gen_tree /home/nathan/Téléchargements/WAF/ /home/nathan/result
Create Tree :
Ok
Convert *.html on *.md :
Ok
Cleaning :
Ok
$ cd /home/nathan/result
$ tree
```

## ⚠️ Possible errors

* The space is not found or is not a directory -> `<PathOfSpace> is not a directory`
* The result already exist -> `<PathOfResult> exist, please enter a folder who doesn't exist`
* Not enough permissions -> `<PathOfResult> is not writable`
* If any error got, the documentation is not generated -> `KO` (Never got to this point)