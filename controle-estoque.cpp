#include <iostream>
#include <vector>
#include <string>
#include <iomanip>  
#include <sstream>
#include <locale>
#include <fstream>
#include <windows.h>

using namespace std;

struct Item {
    string descricao;
    double preco;
    int em_estoque;
    
    Item() : descricao(""), preco(0.0), em_estoque(0) {}
};

vector<Item> itens;
const char* ARQUIVO_DADOS = "inventario.dat";

void carregarDados();
void salvarDados();
void inicializarInventario();
void mostrarInventario();
void editarItem();
void adicionarItem();
void resetarInventario();

void carregarDados() {
    ifstream arquivo(ARQUIVO_DADOS, ios::binary);
    
    if(!arquivo) {
        cout << "Arquivo de dados não encontrado.\n";
        return;
    }
    
    size_t tamanho;
    arquivo.read(reinterpret_cast<char*>(&tamanho), sizeof(tamanho));
    
    itens.resize(tamanho);
    
    for(size_t i = 0; i < tamanho; i++) {
        size_t desc_tamanho;
        arquivo.read(reinterpret_cast<char*>(&desc_tamanho), sizeof(desc_tamanho));
        
        char* buffer = new char[desc_tamanho + 1];
        arquivo.read(buffer, desc_tamanho);
        buffer[desc_tamanho] = '\0';
        itens[i].descricao = string(buffer);
        delete[] buffer;
        
        arquivo.read(reinterpret_cast<char*>(&itens[i].preco), sizeof(itens[i].preco));
        arquivo.read(reinterpret_cast<char*>(&itens[i].em_estoque), sizeof(itens[i].em_estoque));
    }
    
    cout << "Inventário carregado com " << itens.size() << " itens.\n";
}

void salvarDados() {
    ofstream arquivo(ARQUIVO_DADOS, ios::binary);
    
    if(!arquivo) {
        cerr << "Erro ao criar arquivo de dados!\n";
        return;
    }
    
    size_t tamanho = itens.size();
    arquivo.write(reinterpret_cast<const char*>(&tamanho), sizeof(tamanho));
    
    for(size_t i = 0; i < itens.size(); i++) {
        size_t desc_tamanho = itens[i].descricao.size();
        arquivo.write(reinterpret_cast<const char*>(&desc_tamanho), sizeof(desc_tamanho));
        arquivo.write(itens[i].descricao.c_str(), desc_tamanho);
        arquivo.write(reinterpret_cast<const char*>(&itens[i].preco), sizeof(itens[i].preco));
        arquivo.write(reinterpret_cast<const char*>(&itens[i].em_estoque), sizeof(itens[i].em_estoque));
    }
    
    cout << "Dados salvos com sucesso!\n";
}

void inicializarInventario() {
    itens.clear();
    salvarDados();
    cout << "Inventário inicializado com 0 itens padrão.\n";
}

void resetarInventario() {
    cout << "\nTEM CERTEZA QUE DESEJA RESETAR O INVENTÁRIO?\n";
    cout << "Isso apagará todos os dados atuais e criará um novo inventário padrão. (S/N): ";
    
    char confirmacao;
    cin >> confirmacao;
    
    if(toupper(confirmacao) == 'S') {
        inicializarInventario();
        cout << "Inventário resetado com sucesso!\n";
    } else {
        cout << "Operação cancelada.\n";
    }
}

void mostrarInventario() {
    cout << "\nINVENTÁRIO ATUAL:\n";
    cout << "------------------------------------------\n";
    cout << "ID | DESCRICAO         | PRECO   | ESTOQUE \n";
    cout << "------------------------------------------\n";
    
    for(size_t i = 0; i < itens.size(); i++) {
        cout << setw(2) << i+1 << " | " 
             << setw(16) << left << itens[i].descricao << " | "
             << setw(6) << right << fixed << setprecision(2) << itens[i].preco << " | "
             << setw(7) << itens[i].em_estoque << " | "
             << "\n";
    }
}

void editarItem() {
    mostrarInventario();
    cout << "\nDigite o ID do item para editar (0 para cancelar): ";
    int id;
    cin >> id;
    
    if(id == 0) return;
    if(id < 1 || id > static_cast<int>(itens.size())) {
        cout << "ID inválido!\n";
        return;
    }
    
    Item& item = itens[id-1];
    cout << "\nEditando item " << id << ":\n";
    cout << "1. Descrição: " << item.descricao << "\n";
    cout << "2. Preço: R$ " << item.preco << "\n";
    cout << "3. Estoque: " << item.em_estoque << "\n";
    cout << "4. Registrar venda\n";
    cout << "O que deseja alterar? ";
    
    int opcao;
    cin >> opcao;
    cin.ignore();  
    
    switch(opcao) {
        case 1:
            cout << "Nova descrição: ";
            getline(cin, item.descricao);
            salvarDados();
            break;
        case 2:
            cout << "Novo preço: R$ ";
            cin >> item.preco;
            salvarDados();
            break;
        case 3:
            cout << "Novo estoque: ";
            cin >> item.em_estoque;
            salvarDados();
            break;
        
        default:
            cout << "Opção inválida!\n";
    }
}

void adicionarItem() {
    Item novoItem;
    
    cout << "\nADICIONAR NOVO ITEM\n";
    
    do {
        cout << "Descrição: ";
        cin.ignore();
        getline(cin, novoItem.descricao);
        
        if(novoItem.descricao.empty()) {
            cout << "Erro: Descrição não pode ser vazia!\n";
        }
    } while(novoItem.descricao.empty());
    
    do {
        cout << "Preço: R$ ";
        cin >> novoItem.preco;
        
        if(cin.fail() || novoItem.preco <= 0) {
            cin.clear();
            cin.ignore(10000, '\n');
            cout << "Erro: Digite um valor positivo!\n";
        } else {
            break;
        }
    } while(true);
    
    do {
        cout << "Quantidade em estoque: ";
        cin >> novoItem.em_estoque;
        
        if(cin.fail() || novoItem.em_estoque < 0) {
            cin.clear();
            cin.ignore(10000, '\n');
            cout << "Erro: Estoque não pode ser negativo!\n";
        } else {
            break;
        }
    } while(true);
    
    itens.push_back(novoItem);
    salvarDados();
    cout << "Item adicionado com sucesso!\n";
}
void removerItem() {
    if (itens.empty()) {
        cout << "O inventário está vazio!\n";
        return;
    }

    mostrarInventario();
    cout << "\nDigite o ID do item para remover (0 para cancelar): ";
    int id;
    cin >> id;
    
    if (id == 0) return;
    if (id < 1 || id > static_cast<int>(itens.size())) {
        cout << "ID inválido!\n";
        return;
    }
    
    cout << "\nItem a ser removido:\n";
    cout << "Descrição: " << itens[id-1].descricao << "\n";
    cout << "Preço: R$ " << fixed << setprecision(2) << itens[id-1].preco << "\n";
    cout << "Estoque: " << itens[id-1].em_estoque << "\n";
    
    cout << "\nTEM CERTEZA QUE DESEJA REMOVER ESTE ITEM? (S/N): ";
    char confirmacao;
    cin >> confirmacao;
    
    if (toupper(confirmacao) == 'S') {
        itens.erase(itens.begin() + (id-1));
        salvarDados();
        cout << "Item removido com sucesso!\n";
    } else {
        cout << "Operação cancelada.\n";
    }
}
int main() {
    setlocale(LC_ALL, "portuguese");
    
    carregarDados();
    
    while(true) {
        cout << "\nSISTEMA DE INVENTÁRIO\n";
        cout << "1. Mostrar inventário\n";
        cout << "2. Editar item\n";
        cout << "3. Adicionar item\n";
        cout << "4. Remover item\n";
        cout << "9. Resetar inventário\n";
        cout << "0. Sair\n";
        cout << "Escolha uma opção: ";
        
        int opcao;
        cin >> opcao;
        
        switch(opcao) {
            case 1:
                mostrarInventario();
                break;
            case 2:
                editarItem();
                break;
            case 3:
                adicionarItem();
                break;
            case 4:
            	removerItem();
            	break;
            case 9:
                resetarInventario();
                break;
            case 0:
                salvarDados();
                return 0;
            default:
                cout << "Opção inválida!\n";
        }
        
        cout << "\nPressione Enter para continuar...";
        cin.ignore();
        cin.get();
        
        system("cls");
    }
}
