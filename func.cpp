#include"header.h"
// Предварительное объявление класса Marketplace
class Marketplace;

//Стратегия
// Интерфейс для методов оплаты
class PaymentStrategy {
public:
    virtual ~PaymentStrategy() = default;
    virtual void pay(double amount) = 0;
    virtual string getName() const = 0;
};

// Конкретная стратегия оплаты наличными
class CashPayment : public PaymentStrategy {
public:
    void pay(double amount) override {
        cout << "Оплата наличными: " << amount << endl;
    }

    string getName() const override {
        return "Наличные";
    }
};

// Конкретная стратегия оплаты картой
class CardPayment : public PaymentStrategy {
public:
    void pay(double amount) override {
        cout << "Оплата картой: " << amount << endl;
    }

    string getName() const override {
        return "Карта";
    }
};

// Конкретная стратегия оплаты криптовалютой
class CryptoPayment : public PaymentStrategy {
public:
    void pay(double amount) override {
        cout << "Оплата криптовалютой: " << amount << endl;
    }

    string getName() const override {
        return "Криптовалюта";
    }
};

// Класс, описывающий товар
class Product {
private:
    string name;
    double price;
    int kol;
    int sellerId;

public:
    Product(const string& name, double price, int quantity, int sellerId)
        : name(name), price(price), kol(quantity), sellerId(sellerId) {}

    string getName() const { return name; }
    double getPrice() const { return price; }
    int getQuantity() const { return kol; }
    void setQuantity(int newQuantity) { kol = newQuantity; }
    int getSellerId() const { return sellerId; }
};

// Класс, представляющий продавца
class Seller {
private:
    string name;
    int id;
    static int nextId; // Статическая переменная для генерации уникальных ID

public:
    Seller(const string& name) : name(name), id(nextId++) {}

    void addProduct(Marketplace& marketplace, const string& productName, double price, int quantity);

    string getName() const { return name; }
    int getId() const { return id; }
};

int Seller::nextId = 1; // Инициализация статической переменной

// Класс, представляющий покупателя
class Customer {
private:
    string name;
    double balance;

public:
    Customer(const string& name, double balance) : name(name), balance(balance) {}

    string getName() const { return name; }
    double getBalance() const { return balance; }
    void setBalance(double newBalance) { balance = newBalance; }
};

//Компоновщик
// Класс, представляющий торговую площадку
class Marketplace {
public:
    vector<shared_ptr<Seller>> sellers;
    vector<shared_ptr<Customer>> customers;
    vector<shared_ptr<Product>> products;
    shared_ptr<PaymentStrategy> paymentStrategy; // Стратегия оплаты

public:
    void setPaymentStrategy(shared_ptr<PaymentStrategy> strategy) {
        paymentStrategy = strategy;
    }

    shared_ptr<PaymentStrategy> getPaymentStrategy() const {
        return paymentStrategy;
    }

    void addSeller(shared_ptr<Seller> seller) {
        sellers.push_back(seller);
    }

    void addCustomer(shared_ptr<Customer> customer) {
        customers.push_back(customer);
    }

    void addProduct(shared_ptr<Product> product) {
        products.push_back(product);
    }

    string displayProducts() const {
        string result = "Доступные товары:\n";
        for (const auto& product : products) {
            result += "Название: " + product->getName() + ", Цена: " + to_string(product->getPrice())
                      + ", Количество: " + to_string(product->getQuantity()) + "\n";
        }
        return result;
    }

    string buyProduct(string customerName, string productName, int quantity) {
        auto customer = findCustomer(customerName);
        if (!customer) {
            return "Ошибка: Покупатель не найден";
        }

        auto product = findProduct(productName);
        if (!product) {
            return "Ошибка: Товар не найден";
        }

        if (product->getQuantity() < quantity) {
            return "Ошибка: Недостаточно товара на складе";
        }

        double totalCost = product->getPrice() * quantity;
        if (customer->getBalance() < totalCost) {
            return "Ошибка: Недостаточно средств на балансе";
        }

        // Используем стратегию оплаты
        if (paymentStrategy) {
            paymentStrategy->pay(totalCost);
        } else {
            return "Ошибка: Стратегия оплаты не установлена";
        }

        customer->setBalance(customer->getBalance() - totalCost);
        product->setQuantity(product->getQuantity() - quantity);

        return "Покупка совершена. Чек:\n"
               "Товар: " + product->getName() + ", Количество: " + to_string(quantity) + ", Сумма: " + to_string(totalCost) + "\n"
               "Метод оплаты: " + paymentStrategy->getName() + "\n"
               "Остаток на балансе: " + to_string(customer->getBalance());
    }

private:
    shared_ptr<Customer> findCustomer(const string& name) const {
        for (const auto& customer : customers) {
            if (customer->getName() == name) {
                return customer;
            }
        }
        return nullptr;
    }

    shared_ptr<Product> findProduct(const string& name) const {
        for (const auto& product : products) {
            if (product->getName() == name) {
                return product;
            }
        }
        return nullptr;
    }
};

// Реализация метода addProduct в классе Seller
void Seller::addProduct(Marketplace& marketplace, const string& productName, double price, int quantity) {
    auto product = make_shared<Product>(productName, price, quantity, id);
    marketplace.addProduct(product);
}

// Сервер

void ErrorOrNo(int meaning) {
    if (meaning < 0) {
        cerr << "Error\n";
        exit(0);
    }
}

shared_ptr<Seller> findSeller(Marketplace& marketplace, const string& name) {
    for (const auto& seller : marketplace.sellers) {
        if (seller->getName() == name) {
            return seller;
        }
    }
    return nullptr;
}

// Отправление сообщения
void sendAMessage(int system, char* buffer) {
    if (strlen(buffer) < 1) {
        string result = "Неверно введены данные" + '\0';
        for (size_t i = 0; i < result.length(); i++) {
            buffer[i] = result[i];
        }
        send(system, buffer, MAX_SIZE_BUF, 0);
        return;
    }

    string command = buffer;
    istringstream iss(command);
    string cmd;
    iss >> cmd;

    static Marketplace marketplace;

    string result;

    if (cmd == "addProduct") {
        string sellerName, productName;
        double price;
        int quantity;
        iss >> sellerName >> productName >> price >> quantity;
        auto seller = findSeller(marketplace, sellerName);
        if (!seller) {
            result = "Ошибка: Продавец не найден";
        } else {
            seller->addProduct(marketplace, productName, price, quantity);
            result = "Товар добавлен";
        }
    } else if (cmd == "addCustomer") {
        string customerName;
        double balance;
        iss >> customerName >> balance;
        auto customer = make_shared<Customer>(customerName, balance);
        marketplace.addCustomer(customer);
        result = "Покупатель добавлен";
    } else if (cmd == "addSeller") {
        string sellerName;
        iss >> sellerName;
        auto seller = make_shared<Seller>(sellerName);
        marketplace.addSeller(seller);
        result = "Продавец добавлен";
    } else if (cmd == "displayProducts") {
        result = marketplace.displayProducts();
    } else if (cmd == "buyProduct") {
        string customerName, productName, paymentMethod;
        int quantity;
        iss >> customerName >> productName >> quantity >> paymentMethod;

        // Устанавливаем стратегию оплаты в зависимости от метода оплаты
        if (paymentMethod == "Наличные") {
            marketplace.setPaymentStrategy(make_shared<CashPayment>());
        } else if (paymentMethod == "Карта") {
            marketplace.setPaymentStrategy(make_shared<CardPayment>());
        } else if (paymentMethod == "Криптовалюта") {
            marketplace.setPaymentStrategy(make_shared<CryptoPayment>());
        } else {
            result = "Ошибка: Неизвестный метод оплаты";
            for (size_t i = 0; i < result.length(); i++) {
                buffer[i] = result[i];
            }
            buffer[result.length()] = '\0';
            send(system, buffer, MAX_SIZE_BUF, 0);
            return;
        }

        result = marketplace.buyProduct(customerName, productName, quantity);
    } else {
        result = "Ошибка: Неизвестная команда";
    }

    for (size_t i = 0; i < result.length(); i++) {
        buffer[i] = result[i];
    }
    buffer[result.length()] = '\0';

    send(system, buffer, MAX_SIZE_BUF, 0);
}

// Принять сообщение
void AcceptTheMessage(int system, char* buffer) {
    int bytes_received = recv(system, buffer, MAX_SIZE_BUF, 0);
    if (bytes_received <= 0) {
        cerr << "Клиент отключен или произошла ошибка." << endl;
        throw runtime_error("Клиент отключен или произошла ошибка.");
    }
}

// Связь с клиентом
void Communication_With_Client(int server) {
    while (true) {
        static mutex g_lock;
        try {
            char buffer[MAX_SIZE_BUF];
            AcceptTheMessage(server, buffer);
            g_lock.lock();
            cout << "Client" << " : " << endl;
            cout << buffer << endl;
            cout << "Server : " << endl;
            sendAMessage(server, buffer);
            cout << "Запрос выполнен" << endl;
            g_lock.unlock();
        } catch (const exception &e) {
            cerr << e.what() << endl;
            break;
        }
    }
    close(server);
}
