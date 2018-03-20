x1 <- rnorm(100)
x2 <- rnorm(100)
z <-  0.9 + 0.6*tanh(0.03 - 0.3*x1 - 0.2*x2) - 0.4*tanh(0.05 - 0.25*x1 + 0.36*x2)
y <- (z - mean(z))/sd(z)
df <- data.frame(x1, x2, z)
model1 <- lm(z ~ x1 + x2, data=df)
model2 <- lm(y ~ x1 + x2, data=df)
write.table(df, file="patterns.ssv", sep = " ", row.names=FALSE, col.names=FALSE)

