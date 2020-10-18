package itmo.se.lab3.pages

import org.openqa.selenium.*
import org.openqa.selenium.support.FindBy
import org.openqa.selenium.support.PageFactory
import org.openqa.selenium.support.ui.ExpectedConditions.*
import org.openqa.selenium.support.ui.WebDriverWait

class MarketplacePage(private val driver: WebDriver) {
    @FindBy(xpath = "//label[text()=\"Ключевые слова:\"]/following-sibling::input")
    lateinit var keywordsField: WebElement

    @FindBy(xpath = "//form[@name=\"content_form\"]/following-sibling::*/a[text()=\"Искать\"]")
    lateinit var searchButton: WebElement

    @FindBy(xpath = "//span[text()=\"Сортировка: \"]/following-sibling::a[text()=\"цена\"]")
    lateinit var sortByPriceToggle: WebElement

    init {
        driver["https://advego.com/shop/find/"]
        PageFactory.initElements(driver, this)
    }

    fun waitForItems() {
        val itemsLocator = By.xpath("//div[@class=\"list\"]/*")
        WebDriverWait(driver, 8).until(presenceOfElementLocated(itemsLocator))
    }

    inner class Item(private val container: WebElement) {
        val title: String = container.findElement(By.xpath("//a[@class=\"order-title\"]")).text
        val price = container.findElement(By.xpath("//div[@class=\"price\"]")).text.removeSuffix(" руб.").toDouble()
        val keywords =
            container.findElement(By.xpath("//div[starts-with(@id, \"tab-keywords\")]")).text.split(Regex(" \\d+, "))
        private val addToCartButton: WebElement = container.findElement(By.xpath("//a[text()=\"Отложить в корзину\"]"))

        fun addToCart() {
            addToCartButton.click()
            val statusLocator = By.xpath(".//*[contains(text(), \"добавлена в\")]")
            WebDriverWait(driver, 2).until(presenceOfNestedElementLocatedBy(container, statusLocator))
        }
    }

    fun listItems(): List<Item> =
        driver.findElements(By.xpath("//div[starts-with(@class, \"shop_row \")]")).map { Item(it) }
}
