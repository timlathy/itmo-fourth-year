package itmo.se.lab3.pages

import org.openqa.selenium.*
import org.openqa.selenium.support.PageFactory
import org.openqa.selenium.support.ui.ExpectedConditions.*
import org.openqa.selenium.support.ui.WebDriverWait

class MarketplaceCartPage(private val driver: WebDriver) {
    init {
        driver["https://advego.com/shop/my/cart/"]
        PageFactory.initElements(driver, this)
    }

    inner class Item(private val container: WebElement) {
        val title: String = container.findElement(By.xpath(".//a[@class=\"order-title\"]")).text
        private val removeButton = container.findElement(By.xpath(".//a[text()=\"Удалить из корзины\"]"))

        fun removeFromCart() {
            removeButton.click()
            val statusLocator = By.xpath(".//*[contains(text(), \"удалена из корзины\")]")
            WebDriverWait(driver, 2).until(presenceOfNestedElementLocatedBy(container, statusLocator))
        }
    }

    fun getTotalIncludingFee(): Double =
        driver.findElement(By.xpath("//h2[starts-with(text(), \"Корзина /\")]")).text.let {
            Regex("Корзина / ([.\\d]+)").find(
                it
            )!!.groups[1]!!.value.toDouble()
        }

    fun listItems(): List<Item> =
        driver.findElements(By.xpath("//div[starts-with(@class, \"shop_row \")]")).map { Item(it) }
}
