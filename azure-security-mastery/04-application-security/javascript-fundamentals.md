# 🚀 JavaScript Fundamentals for C/C++ Developers

*A quick guide for 42 students transitioning from C/C++ to JavaScript*

## 🎯 Why JavaScript for 42 Projects?

JavaScript is **essential** for modern 42 projects:
- **ft_transcendence**: Frontend (React/Vue) + Backend (Node.js)
- **Web development**: Most web technologies use JS
- **Azure integration**: Azure SDK is excellent in JavaScript
- **Industry standard**: Most startups and companies use JS

---

## 🔄 Key Differences from C/C++

### 1. **No Compilation Step**
```bash
# C/C++
gcc main.c -o program
./program

# JavaScript
node app.js  # Runs directly!
```

### 2. **Dynamic Typing** (No Variable Declarations)
```c
// C/C++
int age = 25;
char* name = "Alice";
bool isStudent = true;
```

```javascript
// JavaScript
let age = 25;
let name = "Alice";
let isStudent = true;

// Variables can change type!
age = "twenty-five";  // Now it's a string!
```

### 3. **Memory Management** (Automatic!)
```c
// C/C++
char* buffer = malloc(100);  // Manual allocation
// ... use buffer ...
free(buffer);                // Manual cleanup
```

```javascript
// JavaScript
let data = new Array(100);   // Automatic allocation
// ... use data ...
// Automatic garbage collection - no free() needed!
```

---

## ⚖️ Triple Equals (===) vs Double Equals (==)

### The Problem with == (Avoid This!)
```javascript
// Double equals does type conversion (confusing!)
console.log(5 == "5");        // true (converts string to number)
console.log(true == 1);       // true (converts boolean to number)
console.log(null == undefined); // true (weird JavaScript behavior)
console.log(0 == false);      // true (converts boolean to number)
```

### The Solution: === (Always Use This!)
```javascript
// Triple equals checks type AND value (like C++ strict comparison)
console.log(5 === "5");        // false (different types)
console.log(true === 1);       // false (different types)
console.log(null === undefined); // false (different types)
console.log(0 === false);      // false (different types)

// Only true when type AND value match
console.log(5 === 5);          // true
console.log("hello" === "hello"); // true
console.log(true === true);    // true
```

### Real-World Example
```javascript
function validateUser(user) {
    // ❌ Dangerous - could accept strings, numbers, etc.
    if (user.age == 18) {
        return "Adult";
    }
    
    // ✅ Safe - only accepts actual number 18
    if (user.age === 18) {
        return "Adult";
    }
}

validateUser({ age: "18" });  // String "18"
// With ==: returns "Adult" (wrong!)
// With ===: doesn't match (correct!)
```

---

## 🔄 Async/Await: Handling Time-Dependent Operations

### Why Async Matters
```c
// C/C++ - Everything happens in order
printf("Step 1\n");
int result = expensive_calculation();  // Blocks until done
printf("Step 2: %d\n", result);
printf("Step 3\n");
```

```javascript
// JavaScript - Network calls don't block
console.log("Step 1");
fetch("https://api.example.com/data")  // Starts request, continues immediately
console.log("Step 2");  // This runs BEFORE the request finishes!
console.log("Step 3");

// Output:
// Step 1
// Step 2  
// Step 3
// (later) API response arrives
```

### The Async/Await Solution
```javascript
// Mark function as async
async function fetchUserData() {
    console.log("Step 1");
    
    // Wait for the network request to complete
    const userData = await fetch("https://api.example.com/user");
    const user = await userData.json();
    
    console.log("Step 2: Got user data");
    console.log("Step 3");
    
    return user;
}

// Output (in order):
// Step 1
// (waits for API)
// Step 2: Got user data
// Step 3
```

### Azure Key Vault Example
```javascript
const { SecretClient } = require("@azure/keyvault-secrets");

async function setupDatabase() {
    // Get database password from Key Vault
    const client = new SecretClient("https://myvault.vault.azure.net/", credential);
    
    // Wait for Key Vault to respond
    const secret = await client.getSecret("database-password");
    const password = secret.value;
    
    // Now connect to database with the password
    const db = await connectToDatabase(password);
    
    return db;
}
```

### Error Handling (Like try/catch in C++)
```javascript
async function safeOperation() {
    try {
        const secret = await client.getSecret("my-secret");
        return secret.value;
    } catch (error) {
        console.error("Failed to get secret:", error.message);
        return null;
    }
}
```

---

## 📦 Variables: let, const, var

### Coming from C/C++:
```c
// C/C++
const int MAX_SIZE = 100;    // Constant
int counter = 0;             // Variable
```

### JavaScript equivalent:
```javascript
// JavaScript
const MAX_SIZE = 100;        // Constant (like C++ const)
let counter = 0;             // Variable (like C++ variable)

// Avoid var (old JavaScript, has weird scoping rules)
```

### Rules:
- **Use `const`** for values that won't change
- **Use `let`** for values that will change
- **Never use `var`** (legacy, causes bugs)

```javascript
const apiUrl = "https://api.example.com";  // Won't change
let userCount = 0;                         // Will change

userCount = 5;     // ✅ OK
apiUrl = "new";    // ❌ Error - can't change const
```

---

## 🏗️ Functions: Several Ways to Write Them

### 1. Traditional Function (Like C++)
```javascript
function calculateTotal(price, tax) {
    return price + (price * tax);
}
```

### 2. Arrow Function (Modern, Concise)
```javascript
const calculateTotal = (price, tax) => {
    return price + (price * tax);
};

// Even shorter for simple operations
const calculateTotal = (price, tax) => price + (price * tax);
```

### 3. Async Functions
```javascript
// Traditional async
async function getSecret(name) {
    return await client.getSecret(name);
}

// Arrow async
const getSecret = async (name) => {
    return await client.getSecret(name);
};
```

---

## 🗂️ Objects and Arrays (No Structs!)

### C/C++ Structs → JavaScript Objects
```c
// C/C++
struct User {
    char name[50];
    int age;
    bool isActive;
};

struct User user = {"Alice", 25, true};
```

```javascript
// JavaScript
const user = {
    name: "Alice",
    age: 25,
    isActive: true
};

// Access properties
console.log(user.name);     // "Alice"
console.log(user["age"]);   // 25 (alternative syntax)

// Add new properties dynamically
user.email = "alice@42.fr";
```

### Arrays (Like C++ vectors)
```javascript
// Create array
const numbers = [1, 2, 3, 4, 5];
const users = ["Alice", "Bob", "Charlie"];

// Access elements
console.log(numbers[0]);    // 1
console.log(users[1]);      // "Bob"

// Useful methods
numbers.push(6);           // Add to end
numbers.pop();             // Remove from end
console.log(numbers.length); // Get size
```

---

## 🔁 Loops and Conditionals

### Similar to C++, but more flexible:
```javascript
// For loop (familiar)
for (let i = 0; i < 10; i++) {
    console.log(i);
}

// For...of loop (like C++ range-based)
const fruits = ["apple", "banana", "orange"];
for (const fruit of fruits) {
    console.log(fruit);
}

// If statements (familiar)
if (user.age >= 18) {
    console.log("Adult");
} else if (user.age >= 13) {
    console.log("Teenager");
} else {
    console.log("Child");
}
```

---

## 🌐 Common Patterns for 42 Projects

### 1. Reading Environment Variables
```javascript
// Like reading command line arguments in C++
const port = process.env.PORT || 3000;
const keyVaultUrl = process.env.KEYVAULT_URL;
```

### 2. HTTP Server (Express.js)
```javascript
const express = require('express');
const app = express();

// Handle GET request (like handling different cases in C++)
app.get('/api/users', async (req, res) => {
    const users = await getUsersFromDatabase();
    res.json(users);
});

app.listen(3000, () => {
    console.log("Server running on port 3000");
});
```

### 3. Working with Promises (Multiple Async Operations)
```javascript
async function loadAllSecrets() {
    // Run multiple Key Vault requests in parallel
    const [dbPassword, apiKey, jwtSecret] = await Promise.all([
        client.getSecret("database-password"),
        client.getSecret("api-key"),
        client.getSecret("jwt-secret")
    ]);
    
    return {
        database: dbPassword.value,
        api: apiKey.value,
        jwt: jwtSecret.value
    };
}
```

---

## 🛠️ Development Tools

### Package Management (Like apt/brew)
```bash
# Initialize project
npm init -y

# Install packages
npm install express @azure/keyvault-secrets @azure/identity

# Run your app
npm start
# or
node app.js
```

### Project Structure
```
my-project/
├── package.json          # Like Makefile + dependencies
├── node_modules/         # Downloaded packages (like libraries)
├── app.js               # Main application file
├── routes/              # Route handlers
├── config/              # Configuration files
└── .env                 # Environment variables (don't commit!)
```

---

## 🚨 Common Gotchas for C++ Developers

### 1. **No Semicolons Required** (But Use Them!)
```javascript
// Works, but don't do this
let x = 5
let y = 10

// Better - be explicit like C++
let x = 5;
let y = 10;
```

### 2. **This Keyword** (Different from C++)
```javascript
const user = {
    name: "Alice",
    greet: function() {
        console.log("Hello, " + this.name);  // 'this' refers to user object
    }
};

user.greet();  // "Hello, Alice"
```

### 3. **Undefined vs Null**
```javascript
let uninitialized;           // undefined (variable exists but no value)
let empty = null;            // null (explicitly set to nothing)

console.log(uninitialized === undefined);  // true
console.log(empty === null);                // true
console.log(undefined === null);            // false (different types!)
```

---

## 🎯 Next Steps for 42 Projects

### Essential Libraries to Learn:
1. **Express.js** - Web server framework
2. **Azure SDK** - Cloud services integration
3. **Mongoose** - MongoDB database
4. **bcrypt** - Password hashing
5. **jsonwebtoken** - JWT authentication

### Recommended Learning Path:
1. ✅ **Basic syntax** (this guide)
2. 🔄 **Async/await** mastery
3. 🌐 **Express.js** web server
4. 🗄️ **Database integration**
5. 🔐 **Azure Key Vault** integration
6. 🚀 **Deployment** to Azure

---

## 💡 Pro Tips

1. **Use strict equality** (`===`) always
2. **Prefer `const` and `let`** over `var`
3. **Use async/await** for cleaner code
4. **Handle errors** with try/catch
5. **Use meaningful variable names** (like good C++ code)
6. **Comment your code** (especially async operations)

---

Ready to build your first JavaScript application with Azure Key Vault? 🚀

*This knowledge will serve you well in ft_transcendence and beyond!*
